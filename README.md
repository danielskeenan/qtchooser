# qtchooser

Manage multiple installed Qt versions.

## Usage

```
Usage: qtchooser [--help] [--version] {choose,list}

Optional arguments:
  -h, --help     shows help message and exits 
  -v, --version  prints version information and exits 

Subcommands:
  choose        Choose a specific Qt installation.
  list          List Qt installations.
```

### list

`qtchooser list`

```
+--------+---------+---------------------------------+
| Active | Version |               Path              |
+--------+---------+---------------------------------+
|        | 6.8.3   | /home/dankeenan/Qt/6.8.3/gcc_64 |
+--------+---------+---------------------------------+
| Active | 6.10.3  | /usr                            |
+--------+---------+---------------------------------+
```

If this program has been used before, the currently chosen installation is highlighted in green.

#### Options:

`--path`
: Additional search paths if you have Qt installations at non-standard locations.

#### Default Search Paths

Windows:

- `C:\Qt` (default path for official Qt installer)

Linux:

- `/usr` (default path for packages distro package managers)
- `/usr/local`
- `~/Qt` (default path for official Qt installer)

### choose

`choose /home/dankeenan/Qt/6.8.3/gcc_64/`

```
Choosing Qt at /home/dankeenan/Qt/6.8.3/gcc_64/
Qt 6.8.3 (x86_64-little_endian-lp64 shared (dynamic) release build; by GCC 10.3.1 20210422 (Red Hat 10.3.1-1)) on "wayland"
An environment variable has changed. You may need to log out and back in for this change to take effect in all programs.
```

On Windows, environment changes are reflected immediately. On other platforms, you must log out and back in for the
change to take effect.
