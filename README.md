# LRA

## Reference

- [Google Cpp Guide](https://google.github.io/styleguide/cppguide.html)

## Goal

- Refactor the src tree
- Implement abstract based class system
- Unify coding style

## Third-Party Libraries

- [BS thread-pool](https://github.com/bshoshany/thread-pool)
- [spdlog](https://github.com/gabime/spdlog)
- [asio](https://github.com/chriskohlhoff/asio.git)
- [jsnocpp](https://github.com/open-source-parsers/jsoncpp.git)
- [websocketpp](https://github.com/zaphoyd/websocketpp.git)

## Todo

- Impl Interface isolation (template specialization ...)

## Dependency

```sh
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt install -y g++-10
sudo apt-get install libboost-all-dev
sudo apt-get install libfftw3-dev
sudo apt install libserial-dev
sudo apt-get install libudev-dev
sudo apt-get install libusb-1.0-0-dev
```

> [!TIP]
> If you're using WSL2, keeping going!

To run the rcws_host application on WSL2, you need to install [usbipd-win](https://github.com/dorssel/usbipd-win). As of March 24, 2025, version 5.0 has been released, and usbipd-win underwent major API updates in version 4.0. The following instructions assume that you are using version ≥ 4.0.

> Setip: Windows Terminal

Please execute the following command in PowerShell:

```
winget install usbipd
```

After installation, you can verify that the device is installed correctly by running:

```
usbipd list
```

You should see an output similar to the following:

<img src="pictures/usbipd_list.png" alt="usbipd_list" style="width:100%;" />

> Setup: WSL2 Terminal

```bash
sudo apt install linux-tools-generic hwdata
sudo update-alternatives --install /usr/local/bin/usbip usbip /usr/lib/linux-tools/*-generic/usbip 20
```

> Attach RCWS to WSL2

Then, execute the following commands (you will need sudo privileges or to run the terminal with administrator rights):

- When binding (make USB device ==SHARED==)

  ```bash
  sudo usbipd bind --busid <bus-id> # or
  sudo usbipd bind -b <bus-id>
  # e.g.,
  # sudo usbipd bind --busid 1-3
  # sudo usbipd bind -b 1-3
  ```

- When attaching to WSL2

  ```bash
  usbipd attach --wsl --busid=<bus-id>
  # e.g.,
  # usbipd attach --wsl --busid=1-3

  # you can use -a flag to automatically re-attach
  # usbipd attach --wsl -a --busid=1-3
  ```

- When detaching from WSL2

  ```bash
  usbipd detach --wsl --busid=<bus-id>
  # e.g.,
  # usbipd detach --wsl --busid=1-3
  ```

- When unbinding
  ```bash
  sudo usbipd unbind --busid <bus-id>
  # e.g.,
  # sudo usbipd unbind --busid 1-3
  ```

Once the connection is successful, start the executable and press `o` — this should successfully open RCWS.

![rcws_open](pictures/rcws_open.png)

## Installation

```sh
git clone --recurse-submodules -j8 https://github.com/DennisLiu16/LRA.git
cd LRA
git checkout usb_test_wsl
```

## Build

```sh
mkdir build
cd build
cmake .. && make
```

## Trouble Shooting

- Encounter `Bad file descriptor` when opening RCWS

  If you encounter `Bad file descriptor` when opening ACM devices, please check following instructions. The issue is caused by tty permissions. Please check if the current user is in the `dialout` group. If not, execute:

  ```bash
  sudo usermod -aG dialout <username>
  ```

  and then log out and log back in or restart the system. This will permanently resolve the issue. As a temporary workaround, you can run the command with `sudo` to elevate permissions.

## Which bin you should run

./build/bin/rcws_host

## Video
https://youtu.be/ZvQ0uNaxThk
