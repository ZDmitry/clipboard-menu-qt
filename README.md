# clipboardMenu
Simple cross-platform tray app for store clippings

This application allow you to store clippings as menu items inside tiny tray app. Click action on the menu item will copy assigned text to clipboard.
Copied text will be available for paste within `5 sec`. You can disable timeout with setting `security` (by default - `true`) or adjust `timeout` options (in msec).

###### Command-line options

Supported command-line options:
- `--file` or `-f` &lt;json_file_path&gt; - path to json menu (by default: `menu.json` from app directory).
- `--url`  or `-u` &lt;remote__url&gt; - url to json menu.

###### Menu structure

Example of `menu.json`:
```
{
  "menu": {
    "one": {
      "clip1": "some text...",
      "clip2": "some text..."
    },
    "two": {
      "clip1": "some text...",
      "clip2": "some text..."
    }
  },
  "settings": {
    "secure":  true,
    "timeout": 5000
  }
}
```
