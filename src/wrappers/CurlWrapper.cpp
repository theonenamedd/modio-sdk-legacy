#include "wrappers/CurlWrapper.h"

namespace modio
{
  namespace curlwrapper
  {
    CURLM *curl_multi_handle;

    map<CURL*, JsonResponseHandler*> ongoing_calls;

    CurrentDownloadHandle* current_download_handle;

    CurrentDownloadInfo current_download_info;

    int call_count = 0;
    int ongoing_call = 0;

    void initCurl()
    {
      current_download_handle = new CurrentDownloadHandle;
      current_download_handle->path = "";
      current_download_handle->pause_flag = false;
      current_download_handle->curl = NULL;

      current_download_info.url = "";
      current_download_info.download_total = 0;
      current_download_info.download_progress = 0;

      curl_multi_handle = curl_multi_init();

      if(curl_global_init(CURL_GLOBAL_ALL) == 0)
        writeLogLine("Curl initialized", MODIO_DEBUGLEVEL_LOG);
      else
        writeLogLine("Error initializing curl", MODIO_DEBUGLEVEL_ERROR);
    }

    int getCallCount()
    {
      return call_count;
    }

    int getOngoingCall()
    {
      return ongoing_call;
    }

    void advanceCallCount()
    {
      call_count++;
    }

    void advanceOngoingCall()
    {
      ongoing_call++;
    }

    void lockCall(int call_number)
    {
      while(call_number!=getOngoingCall());
    }

    JsonResponseHandler::JsonResponseHandler(int call_number, function<void(int call_number, int response_code, json response_json)> callback)
    {
      this->response = "";
      this->call_number = call_number;
      this->callback = callback;
    }

    size_t get_data(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
      CURL* handle = (CURL*)userdata;
      int data_size = size * nmemb;
      ongoing_calls[handle]->response.append(ptr, data_size);
      return data_size;
    }

    void setHeaders(vector<string> headers, CURL* curl)
    {
      struct curl_slist *chunk = NULL;
      for(int i=0;i<(int)headers.size();i++)
        chunk = curl_slist_append(chunk, headers[i].c_str());

      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    }

    void setVerifies(CURL* curl)
    {
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }

    void setJsonResponseWrite(CURL* curl)
    {
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_data);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, curl);
    }

    json parseJsonResonse(string response)
    {
      if(response == "")
        return "{}"_json;

      json response_json;
      try
      {
        response_json = json::parse(response);
      }
      catch (json::parse_error &e)
      {
        writeLogLine(string("Error parsing json: ") + e.what(), MODIO_DEBUGLEVEL_ERROR);
        response_json = "{}"_json;
      }
      return response_json;
    }

    void get(int call_number, string url, vector<string> headers, function<void(int call_number, int response_code, json response_json)> callback)
    {
      writeLogLine("getJsonCall call to " + url, MODIO_DEBUGLEVEL_LOG);
      //lockCall(call_number);
      CURL *curl;

      curl_global_init(CURL_GLOBAL_DEFAULT);

      curl = curl_easy_init();

      ongoing_calls[curl] = new JsonResponseHandler(call_number, callback);
      if(curl)
      {
        setHeaders(headers, curl);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        setVerifies(curl);
        setJsonResponseWrite(curl);

        curl_multi_add_handle(curl_multi_handle, curl);
      }
    }

    size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
    {
      size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
      return written;
    }

    int progress_callback(void *clientp,   double dltotal,   double dlnow,   double ultotal,   double ulnow)
    {
      current_download_info.download_total = dltotal;
      current_download_info.download_progress = dlnow;

      if(current_download_handle->pause_flag)
      {
        curl_easy_pause(current_download_handle->curl , CURLPAUSE_RECV);

        string file_path = getModIODirectory() + "paused_download.json";

        json paused_download_json;

        paused_download_json["path"] = current_download_handle->path;

        time_t  timev;
        time(&timev);
        paused_download_json["time"] = timev;

        paused_download_json["download_total"] = dltotal;
        paused_download_json["download_progress"] = dlnow;

        std::ofstream o(file_path);
        o << std::setw(4) << paused_download_json << std::endl;
        return -1;
      }

      if(dltotal != 0 && dltotal == dlnow)
      {
        if(current_download_handle)
        {
          current_download_handle->path = "";
          current_download_handle->pause_flag = false;
          current_download_handle->curl = NULL;
        }

        current_download_info.url = "";
        current_download_info.download_total = 0;
        current_download_info.download_progress = 0;
      }

      return 0;
    }

    void pauseCurrentDownload()
    {
      string path = current_download_handle->path;

      if(path != "")
      {
        string extension = path.substr(path.length() - 4);

        for(int i=1; i<(int)extension.size();i++)
          extension[i] = tolower(extension[i]);

        if(extension == ".zip")
        {
          current_download_handle->pause_flag = true;
        }
      }
    }

    void shutdownCurl()
    {
      pauseCurrentDownload();

      ongoing_call = -1;
      call_count = -1;
      ongoing_calls.clear();
    }

    curl_off_t getProgressIfStored(string path)
    {
      string file_path = getModIODirectory() + "paused_download.json";
      std::ifstream in(file_path);
      json modfile_downloads_json;
      if(in.is_open())
      {
        in>>modfile_downloads_json;
        string path_stored = modfile_downloads_json["path"];
        curl_off_t download_progress_stored = modfile_downloads_json["download_progress"];
        if(path_stored == path)
        {
          return download_progress_stored;
        }
      }
      return 0;
    }

    CurrentDownloadInfo getCurrentDownloadInfo()
    {
      return current_download_info;
    }

    void download(int call_number, string url, string path, FILE* file, curl_off_t progress, function<void(int call_number, int response_code, json response)> callback)
    {
      writeLogLine("downloadFile call to " + url, MODIO_DEBUGLEVEL_LOG);
      //lockCall(call_number);
      CURL *curl;
      curl = curl_easy_init();

      current_download_handle->path = path;
      current_download_handle->pause_flag = false;
      current_download_handle->curl = curl;

      current_download_info.url = url;

      ongoing_calls[curl] = new JsonResponseHandler(call_number, callback);

      if(progress!=0)
      {
        curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, progress);
      }

      if(curl)
      {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        setVerifies(curl);

        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        curl_multi_add_handle(curl_multi_handle, curl);
      }
    }

    void postForm(int call_number, string url, vector<string> headers, multimap<string, string> curlform_copycontents, map<string, string> curlform_files, function<void(int call_number, int response_code, json response)> callback)
    {
      writeLogLine(string("postForm call to ") + url, MODIO_DEBUGLEVEL_LOG);
      //lockCall(call_number);
      CURL *curl;

      struct curl_httppost *formpost=NULL;
      struct curl_httppost *lastptr=NULL;
      struct curl_slist *headerlist=NULL;
      static const char buf[] = "Expect:";

      curl_global_init(CURL_GLOBAL_ALL);

      for(map<string,string>::iterator i = curlform_files.begin();
            i!=curlform_files.end();
            i++)
      {
        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, (*i).first.c_str(),
          CURLFORM_FILE, (*i).second.c_str(), CURLFORM_END);
      }

      for(map<string,string>::iterator i = curlform_copycontents.begin();
            i!=curlform_copycontents.end();
            i++)
      {
        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, (*i).first.c_str(),
          CURLFORM_COPYCONTENTS, (*i).second.c_str(), CURLFORM_END);
      }

      /* Fill in the submit field too, even if this is rarely needed */
      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "submit",
                   CURLFORM_COPYCONTENTS, "send",
                   CURLFORM_END);

      curl = curl_easy_init();

      ongoing_calls[curl] = new JsonResponseHandler(call_number, callback);

      setHeaders(headers, curl);

      /* initialize custom header list (stating that Expect: 100-continue is not
         wanted */
      headerlist = curl_slist_append(headerlist, buf);
      if(curl)
      {
        /* what URL that receives this POST */
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        setVerifies(curl);
        setJsonResponseWrite(curl);

        //if((argc == 2) && (!strcmp(argv[1], "noexpectheader")))
          /* only disable 100-continue header if explicitly requested */
          //curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        curl_multi_add_handle(curl_multi_handle, curl);

        //curl_easy_cleanup(curl);
        //curl_formfree(formpost);
        //curl_slist_free_all(headerlist);
      }
    }

    void post(int call_number, string url, vector<string> headers, map<string, string> data, function<void(int call_number, int response_code, json response_json)> callback)
    {
      writeLogLine(string("post call to ") + url, MODIO_DEBUGLEVEL_LOG);
      //lockCall(call_number);

      CURL *curl;

      curl_global_init(CURL_GLOBAL_ALL);
      curl = curl_easy_init();

      ongoing_calls[curl] = new JsonResponseHandler(call_number, callback);

      if(curl)
      {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        setHeaders(headers, curl);

        string str_data = "";
        for(map<string, string>::iterator i = data.begin(); i!=data.end(); i++)
        {
          if(i!=data.begin())
            str_data += "&";
          str_data += (*i).first + "=" + (*i).second;
        }

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());

        setVerifies(curl);
        setJsonResponseWrite(curl);

        curl_multi_add_handle(curl_multi_handle, curl);
      }
    }

    void put(int call_number, string url, vector<string> headers, multimap<string, string> curlform_copycontents, function<void(int call_number, int response_code, json response_json)> callback)
    {
      writeLogLine(string("put call to ") + url, MODIO_DEBUGLEVEL_LOG);
      //lockCall(call_number);
      CURL *curl;

      curl_global_init(CURL_GLOBAL_ALL);

      curl = curl_easy_init();

      ongoing_calls[curl] = new JsonResponseHandler(call_number, callback);

      if(curl)
      {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

        headers.push_back("Content-Type: application/x-www-form-urlencoded");
        setHeaders(headers, curl);

        string str_data = "";
        for(map<string, string>::iterator i = curlform_copycontents.begin(); i!=curlform_copycontents.end(); i++)
        {
          if(str_data!="")
            str_data += "&";
          str_data += (*i).first + "=" + (*i).second;
        }

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str_data.c_str());

        setVerifies(curl);
        setJsonResponseWrite(curl);

        curl_multi_add_handle(curl_multi_handle, curl);
      }
    }

    void deleteCall(int call_number, string url, vector<string> headers, function<void(int call_number, int response_code, json response_json)> callback)
    {
      writeLogLine(string("delete call to ") + url, MODIO_DEBUGLEVEL_LOG);
      //lockCall(call_number);
      CURL *curl;

      curl_global_init(CURL_GLOBAL_ALL);
      curl = curl_easy_init();

      ongoing_calls[curl] = new JsonResponseHandler(call_number, callback);

      if(curl)
      {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

        headers.push_back("Content-Type: application/x-www-form-urlencoded");
        setHeaders(headers, curl);
        setVerifies(curl);
        setJsonResponseWrite(curl);

        curl_multi_add_handle(curl_multi_handle, curl);
      }
    }

    void process()
    {
      CURLMcode code;
      int handle_count;
      code = curl_multi_perform(curl_multi_handle, &handle_count);

      struct CURLMsg *curl_message;

      do
      {
        int msgq = 0;
        curl_message = curl_multi_info_read(curl_multi_handle, &msgq);
        if(curl_message && (curl_message->msg == CURLMSG_DONE)) {
          CURL *curl_handle = curl_message->easy_handle;

          if(ongoing_calls.find(curl_handle) != ongoing_calls.end())
          {
            json response_json = parseJsonResonse(ongoing_calls[curl_handle]->response);
            int response_code;
            curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
            ongoing_calls[curl_handle]->callback(ongoing_calls[curl_handle]->call_number, response_code, response_json);
            advanceOngoingCall();
          }

          curl_multi_remove_handle(curl_multi_handle, curl_handle);
          curl_easy_cleanup(curl_handle);
        }
      } while(curl_message);
    }
  }
}
