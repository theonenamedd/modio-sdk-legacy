<a href="https://mod.io"><img src="https://static.mod.io/v1/images/branding/modio-color-dark.svg" alt="mod.io" width="360" align="right"/></a>
# mod.io SDK
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://github.com/modio/SDK/blob/master/LICENSE)
[![Discord](https://img.shields.io/discord/389039439487434752.svg?label=Discord&logo=discord&color=7289DA&labelColor=2C2F33)](https://discord.mod.io)
[![Master docs](https://img.shields.io/badge/docs-master-green.svg)](https://github.com/modio/SDK/wiki)
[![Travis](https://img.shields.io/travis/modio/SDK.svg?logo=travis)](https://travis-ci.org/modio/SDK)

Welcome to the [mod.io SDK](https://apps.mod.io/sdk) repository, built using C and C++. It allows game developers to host and automatically install user-created mods in their games. It connects to the [mod.io API](https://docs.mod.io), and [documentation for its functions](https://github.com/modio/SDK/wiki) can be viewed here.

## Usage

### Browse mods

```
modio::FilterCreator filter_creator;
filter_creator.setLimit(5); // limit the number of results
filter_creator.setOffset(0); // paginate through the results by using a limit and offset together
filter_creator.setSort("date_updated", false); // the filtering system allows flexible queries

modio_instance.getAllMods(filter_creator, [&](const modio::Response& response, const std::vector<modio::Mod> & mods)
{
  if(response.code == 200)
  {
    // Mod data retreived!
  }
});
```

### Auth

Authentication enables automatic installs and updates under the hood.

First step is to request a security code to your email.

```
modio_instance.emailRequest("example@mail.com", [&](const modio::Response& response)
{
  if (response.code == 200)
  {
    // Authentication code successfully sent to the e-mail
  }
});
```

Finish authentication by submitting the 5-digit code.

```
modio_instance.emailExchange("50AD4", [&](const modio::Response& response)
{
  if (response.code == 200)
  {
    // Email exchanged successfully, you are now authenticated
  }
});
```

### External Auth

If your game is running inside a popular distribution platform such as Steam or GOG Galaxy you can authenticate 100% seamlessly.

#### Galaxy Auth

```
modio_instance.galaxyAuth(appdata, [&](const modio::Response &response)
{
  if (response.code == 200)
  {
    // Successful Galaxy authentication
  }
});
```

#### Steam Auth

```
modio_instance.steamAuth(rgubTicket, cubTicket, [&](const modio::Response &response)
{
  if (response.code == 200)
  {
    // Successful Steam authentication
  }
});
```

### Subscriptions

Download mods automatically by subscribing, uninstall them by unsubscribing.

#### Subscribe

```
modio_instance.subscribeToMod(mod_id, [&](const modio::Response& response, const modio::Mod& mod)
{
  if(response.code == 201)
  {
    // Subscribed to mod successfully, the mod will be added automatically to the download queue
  }
});
```

#### Unsubscribe

```
modio_instance.unsubscribeFromMod(mod_id, [&](const modio::Response& response)
{
  if(response.code == 204)
  {
    // Unsubscribed from mod successfully, it will be uninstalled from local storage
  }
});
```

### Mod submission

Share mods by creating a mod profile and attaching modfiles to it.

#### Create a mod profile

```
modio::ModCreator mod_creator;
mod_creator.setName("Graphics Overhaul Mod");
mod_creator.setLogoPath("path/to/image.jpg");
mod_creator.setHomepage("http://www.garphicsoverhaulmod.com");
mod_creator.setSummary("Short descriptive summary here.");
mod_creator.addTag("Graphics");
mod_creator.addTag("HD");

modio_instance.addMod(mod_creator, [&](const modio::Response& response, const modio::Mod& mod)
{
  if(response.code == 201)
  {
    // Mod profile successfully added
  }
});
```

#### Upload a modfile

```
modio::ModfileCreator modfile_creator;
modfile_creator.setModfilePath("path/to/mod_folder/");
modfile_creator.setModfileVersion("v1.1.0");
modfile_creator.setModfileChangelog("<p>Rogue Knights v1.2.0 Changelog</p></p>New Featu...");

modio_instance.addModfile(requested_mod.id, modfile_creator);
```

### Listeners

#### Download listener

```
modio_instance.setDownloadListener([&](u32 response_code, u32 mod_id) {
  if (response_code == 200)
  {
    // Mod successfully downloaded. Call the modio_instance.installDownloadedMods(); to install, keep in mind this is not an async function.
  }
});
```

#### Upload listener

```
modio_instance.setUploadListener([&](u32 response_code, u32 mod_id) {
  if (response_code == 201)
  {
    //Mod successfully uploaded
  }
});
```

### Feature rich integration

Visit the [wiki](https://github.com/modio/SDK/wiki) to learn how to integrate dependencies, comments, ratings, stats, reports and more.

## Getting started

If you are a game developer, first step is to add mod support to your game. Once mod support is up and running, [create your games profile](https://mod.io/games/add) on mod.io, to get an API key and access to all [functionality mod.io offers](https://apps.mod.io/guides/getting-started).
Next, download the latest [SDK release](https://github.com/modio/SDK/releases) and unpack it into your project, then head over to the [GitHub Wiki](https://github.com/modio/SDK/wiki/Getting-Started) and follow the guides corresponding to your setup.

```
#include "modio.h"

// ...

modio::Instance modio_instance(MODIO_ENVIRONMENT_TEST, MY_GAME_ID, "MY API KEY");

// ...

void myGameTick()
{
  modio_instance.process();
}
```


## Contributions Welcome
Our SDK is public and open source. Game developers are welcome to utilize it directly, to add support for mods in their games, or fork it to create plugins and wrappers for other engines and codebases. Many of these [contributions are shared](https://apps.mod.io) here. Want to make changes to our SDK? Submit a pull request with your recommended changes to be reviewed.

## Wrappers

mod.io SDK wrappers are available for the following languages and engines:

* [Haxe wrapper](https://apps.mod.io/haxe-wrapper) for game engines such as OpenFL, Kha or awe5. Available on [Github](https://github.com/Turupawn/modioHaxe).
* [Unreal Engine](https://apps.mod.io/unreal-engine-plugin) integration tutorial and plugin on the works.

Are you creating a wrapper? [Let us know](http://discord.mod.io)!

### Building instructions

Learn how to build mod.io SDK in our [building instruction guide](https://github.com/modio/SDK/wiki/Building).

### Code contributions

1. Fork it
2. Add new features
```bash
git checkout -b my-new-feature
git commit -am 'Add some feature'
git push origin my-new-feature
```
3. Create a pull request

### Reporting a bug

If you're unable to find an open issue addressing the problem, [open a new one](https://github.com/modio/SDK/issues). Be sure to include a title and clear description, as much relevant information as possible, and a code sample or an executable test case demonstrating the expected behavior that is not occurring.

## Other Repositories
Our aim with [mod.io](https://mod.io), is to provide an open modding API. You are welcome to [view, fork and contribute to our other codebases](https://github.com/modio) in use:

* [Design](https://design.mod.io) is public and open source, the repository can be [found here](https://github.com/modio/WebDesign).
* [API documentation](https://docs.mod.io) is public and open source, the repository can be [found here](https://github.com/modio/APIDocs).
* [Browse engine tools](https://apps.mod.io), plugins and wrappers created by the community, or [share your own](https://apps.mod.io/add).
* [Unreal Engine 4 plugin](https://github.com/modio/UE4Plugin), easily manage the browsing and install of mods in Unreal Engine 4 games
* [Unity plugin](https://github.com/modio/UnityPlugin), easily manage the browsing and install of mods in Unity Engine games
* [Python wrapper](https://github.com/ClementJ18/mod.io), a python wrapper for the mod.io API
* [Rust wrapper](https://github.com/nickelc/modio-rs), rust interface for mod.io
* And more...
