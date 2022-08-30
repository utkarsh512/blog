# Kernel configuration

*Part of an assignment for the course CS60038: Advances in Operating System Design*

## Remove AppArmor support

### What is AppArmor?

AppArmor protects Linux systems from insecure or untrusted processes by running them in restricted
confinement, while still allowing processes to share files, exercise privilege, and communicate with other
processes. You can check whether the kernel version you have includes AppArmor support by using `cat /sys/kernel/security/apparmor/profile`.

### How to remove AppArmor support

* Run `make menuconfig`. This will open kernel configuration menu.
* Activate search box by pressing `/`.
* Search for `apparmor`.
* Choose the option for which `SYMBOL` is `SECURITY_APPARMOR` (by pressing the option number).
* Cursor will now move to highlighted part `[*] AppArmor support`. `[*]` means that AppArmor is currently supported.
* Press `N` to de-select.
* Save the configuration. 
* Press `ESC-ESC` to exit.

### What has changed?

Before this configuration, `.config` file should look like this:

```bash
...
CONFIG_SECURITY_APPARMOR=y
CONFIG_SECURITY_APPARMOR_HASH=y
CONFIG_SECURITY_APPARMOR_HASH_DEFAULT=y
# CONFIG_SECURITY_APPARMOR_DEBUG is not set
...
```

After removing the support, `.config` file should look like this:

```bash
...
# CONFIG_SECURITY_APPARMOR is not set
...
```

## Remove DCCP support

### What is DCCP?

DCCP is a general-purpose transport protocol that provides the establishment and maintenance of
unreliable packet flow and congestion control. Use the following command for checking if DCCP is
disabled: `grep -r dccp /etc/modprobe.d/* | grep -i "/bin/true"`.

### How to remove DCCP support

* DCCP, by default, is disabled in the kernel distribution. But, for this example, we assume that it was previously enabled by the user.
* Run `make menuconfig`. This will open kernel configuration menu.
* Activate search box by pressing `/`.
* Search for `IP_DCCP`.
* Choose the option for which `SYMBOL` is `IP_DCCP` (by pressing the option number).
* Cursor will now move to highlighted part `<*> The DCCP protocol`. `<*>` means that DCCP is currently enabled.
* Press `N` to disable.
* Save the configuration. 
* Press `ESC-ESC` to exit.

### What has changed?

Before this configuration, `.config` file should look like this:

```bash
...
# CONFIG_IP_DCCP is not set
...
```

After removing the support, `.config` file should look like this:

```bash
...
CONFIG_IP_DCCP=y
...
```

## Update default TCP congestion control algorithm to Reno

### What is TCP Reno?

TCP Reno detects lost packets early as half of the current congestion window is saved and it skips slow
start by going directly to the congestion avoidance algorithm. To check the current TCP congestion
control algorithm, use the command: `cat /proc/sys/net/ipv4/tcp_congestion_control`

### How to update default TCP congestion control algorithm to Reno

* The default TCP congestion control algorithm is `Cubic`. 
* Run `make menuconfig`. This will open kernel configuration menu.
* Follow: Networking support -> Networking options -> TCP/IP networking -> TCP advanced congestion control -> Default TCP congestion control.
* Cursor will now move to highlighted part `(X) Cubic`. 
* Press down-arrow key and ENTER. This will select `( ) Renu` option.
* Save the configuration. 
* Press `ESC-ESC` to exit.

### What has changed?

Before this configuration, `.config` file should look like this:

```bash
...
CONFIG_DEFAULT_CUBIC=y
# CONFIG_DEFAULT_RENO is not set
...
```

After removing the support, `.config` file should look like this:

```bash
...
# CONFIG_DEFAULT_CUBIC is not set
CONFIG_DEFAULT_RENO=y
...
```
