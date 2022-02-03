# YAITAA - Yet Another Image To A(NSI) Art (converter).

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/9fdbd9f8bc7843df9a6715b72b4da2fd)](https://www.codacy.com/gh/hatkidchan/yaitaa/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=hatkidchan/yaitaa&amp;utm_campaign=Badge_Grade)

Yes, very original. Old name is `asciify-reborn` for reasons stated bellow

## Reasons for reimplementation

Everything is already described in
[hatkidchan/asciify](https://github.com/hatkidchan/asciify#readme), but just as
recap:

1. I'm not happy with old project structure
2. Some new features may be pain in the ass to add
3. Spaghetti code
4. Literally dublication
5. Separate binaries for each mode
6. Git submodules. Yay.


## Build instructions
### 1. Make sure that you have C compiler and make

```sh
cc --version && make --version && echo 'ok'
```

Both of commands should succeed.

### 2. Clone repo

```sh
git clone --recursive https://github.com/hatkidchan/yaitaa
cd yaitaa
```

### 2. Build it

```sh
make
```

### 3. Run it

```sh
./yaitaa -M braille -S 256 -W 80 -H 24 ~/images/kitty.png
```

### Build end notes

#### 1. It is preferred to use `clang` as your compiler

Main reason being that it is more strict, so any issues I've missed may be
reported on that step. Feel free to send them to me!

```sh
CC=clang make
```

#### 2. Strip binary to reduce size (saves ~100KiB)

```sh
strip ./yaitaa
```

#### 3. Installation

It's not really designed to be installed system-wide, but if you *really* want
to do so, do the following

```sh
# as root (using sudo/doas/etc.)
cp ./yaitaa /usr/local/bin/yaitaa
```

Alternatively, you can install it to your local directory, if it's in your
`$PATH`:

```sh
cp ./yaitaa ~/.local/bin/yaitaa
```

## License

```text
yaitaa - Yet Another Image To A(NSI) Art (converter)
Copyright (C) 2022 hatkidchan <hatkidchan at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
```
