# Alligator

Alligator is a convergent RSS/Atom feed reader.

<a href='https://flathub.org/apps/details/org.kde.alligator'><img width='190px' alt='Download on Flathub' src='https://flathub.org/assets/badges/flathub-badge-i-en.png'/></a>

![Screnshot of a feed](https://cdn.kde.org/screenshots/alligator/alligator-desktop.png)

## Get It

Details on where to find stable releases of Alligator can be found on its
[homepage](https://apps.kde.org/alligator). Nightly Android APKs are available in KDE's [F-Droid repositories](https://community.kde.org/Android/F-Droid).

## Building

Note: When using versions of alligator built from git-master, it's possible that the database format or the name of downloaded files change from one version to another without the necessary migrations to handle it. If you notice bugs after upgrading to a git-master version, export your feeds, delete `~/.local/share/KDE/alligator` and import the feeds again.

### Requirements
 - KCoreAddons
 - KI18n
 - KConfig
 - Kirigami
 - Syndication

### Linux

```
git clone https://invent.kde.org/plasma-mobile/alligator
cd alligator
mkdir build && cd  build
cmake .. -DCMAKE_PREFIX_PATH=/usr
make
sudo make install
```

This assumes all dependencies are installed. If your distribution does not provide
them, you can use [kdesrc-build](https://kdesrc-build.kde.org/) to build all of them.

### Android

You can build Alligator for Android using KDE's [Docker-based build environment](https://community.kde.org/Android/Environment_via_Container).

