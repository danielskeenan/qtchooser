# qtchooser

Manage multiple installed Qt versions.

## Usage

```
qtchooser list [--path <PATH>...]
```

```
qtchooser choose <PREFIX>
```

### list

Lists the Qt installations found on the system. By default, this will look in the following directories:

- `/usr`
- `/usr/local`
- `~/Qt`

Use the `--path` option to add additional paths to search. Both POSIX prefixes and official Qt Installer
directories are supported.

```
$ qtchooser list
+--------+---------+----------------------------+
| Active | Version |            Path            |
+--------+---------+----------------------------+
|        | 6.8.3   | /home/user/Qt/6.8.3/gcc_64 |
+--------+---------+----------------------------+
| Active | 6.10.3  | /usr                       |
+--------+---------+----------------------------+
```

The currently chosen installation is highlighted in green.

#### Options:

`--path`
: Additional search paths if you have Qt installations at non-standard locations. Pass more than once to add more
than one path.

#### Default Search Paths

Windows:

- `C:\Qt` (default path for official Qt installer)

Linux:

- `/usr` (default path for distro package managers)
- `/usr/local`
- `~/Qt` (default path for official Qt installer)

### choose

Update system environment variables to use the Qt installation in the given prefix.

The following variables are updated:

`Qt6_DIR`
: Path to the chosen Qt's CMake package file

`Qt_DIR`
: Same as Qt6_DIR

`QT_ROOT_DIR`
: Qt install prefix

`PATH`
: Updates ~/.bash_profile to prefix Qt's executable directories to the PATH.

```
$ qtchooser choose /home/user/Qt/6.8.3/gcc_64/
Choosing Qt at /home/user/Qt/6.8.3/gcc_64
An environment variable has changed. You may need to log out and back in for this change to take effect in all programs.
```

On Windows, environment changes are reflected immediately. On other platforms, you must log out and back in for the
change to take effect.

## License

Copyright (C) 2026 Dan Keenan

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [GNU General Public License](LICENSE.md) for
more details.

You should have received a copy of the GNU General Public License along with this program. If not,
see <https://www.gnu.org/licenses/>.
