# Piggybacking Scripts

Piggybacking is an *extremely* lightweight approach to cluster management. Together with a [reservation spreadsheet](https://goo.gl/zHG5uT) it helps to increase resource utilization. If you're managing a large cluster, consider instead a job queue manager ([HTCondor](http://htcondor.org/), [slurm](https://slurm.schedmd.com/), etc.), but where those solutions are cumbersome, Piggybacking provides a simpler alternative.

**Please note this is an alpha public release**, still rough around the edges. Please, help us to improve it, by [alerting us of any issues](https://github.com/recodbr/piggybacking/issues). Unfortunately, we cannot provide technical support (e.g., help on installation, usage, dependencies, etc.), but we'll do our best to get rid of bugs in our own code. Note, however, that this code comes with **no warranties of any kind** (see LICENCE).

## About

Piggybacking serves a single purpose: launching a process, and granting other users the power to kill it. The use case at [RECOD](https://recodbr.wordpress.com/) is sharing computing resources. Most of our cluster is managed with HTCondor, but we find it cumbersome for some of the nodes — in particular the GPU nodes used for Deep Learning — which we prefer to manage with manual reservations.

Reserved nodes may stay idle for several hours, during which other users could run short interactive sessions, or even small batch jobs — i.e., they could *piggyback* on idle reservations — provided the primary owner could reclaim the node as soon as it is needed. Piggybacking provides a mechanism for such reclamation.

## Installation

Installation is straighforward. Clone or download this repository. If you're installing for all users in your system, move scripts piggyback_launch and piggyback_kill to /usr/bin (or other adequate directory in everyone's PATH). If you're installing just for yourself, add those scripts to a directory in your home (e.g., ~/bin) and add that directory to your PATH (e.g., appending ```export PATH="$HOME/bin;$PATH"``` to your ~/.profile or ~/.bash_profile.

The package contains an utility ```test_process``` that can be used to test the installation. To compile it, run:

```bash
gcc test_process.c -o test_process -Wall
```

## How to use

To launch a killable process, use ```piggyback_launch <command> [ arguments... ]```. For example:

```bash
someuser@reserved_node$ piggyback_launch piggyback_launch ./test_process 60 --ignore-sigterm "dummy argument 1" dummy_argument_2 'dummy argument 3'
```

This will launch the command with the arguments and output the code and password needed to kill the process, for example:

```
piggyback: The code and password to kill your process are:
piggyback:     CODE       PASSWORD
piggyback:     vnqlzROm0v DWIBHWDHVQ
piggyback: WARNING: anyone with those codes can kill your process!
piggyback: Your process (pid=90890) has launched
```

The primary owner of ```reserved_node``` (or anyone else in possession of the codes above) can kill the process with ```piggyback_kill <piggybacker> <kill code> <kill password>```. For example:

```bash
anotheruser@reserved_node$ piggyback_kill someuser vnqlzROm0v DWIBHWDHVQ
```

After piggyback_kill is called, the process will be sent a SIGTERM signal, which allows for graceful termination (e.g., deleting temporary files, saving checkpoints, etc.), and will have a grace period of 5 minutes for those cleanup activities. If after 5 minutes, the process is still alive, it will be sent the uncatchable SIGKILL signal and killed immediately.

### Redirections and pipelines

Redirections and pipes are managed by the shell. If you need to redirect stdin/stdout/stderr for a command, or to launch a complex pipeline, you should launch a batch subshell and use the redirection / pipeline as an argument. Shell built-ins can also be called in that way. For example:

```bash
./piggyback_launch -ct bash -c 'find / -iname '\''*.png'\'' 2> /dev/null | tr "a-z" "A-Z" > images'
```

The ```-ct``` option (see below) is necessary because bash, by default, does not terminate its children processes.

### Launching interactive sessions (experimental)

Launching interactive sessions, like interpreters, or subshells is not recommended as it tends to cause problems for both the piggybacker (unexpected behaviours on the sessions), and the primary owner (unreliable process killing).

Nevertheless, as an experimental feature, Piggybacking provides the ```-i``` option for such launches. You can use, for example, ```piggyback_launch -i python``` to launch a Python interactive session.

### Child processes

Usually, only the parent process will be sent the SIGTERM signal (for graceful termination), as piggyback_launch will consider the parend responsible for terminating its children before finishing. If forceful termination is needed, however, the children will be sent SIGKILL before the parent. Those defaults can be modified with the options ```-ct``` (do send SIGTERM to children before the parent), and ```-ck-``` (don't send SIGKILL to the children).

In particular, if you need to launch subshells (batch or interactive), ```-ct``` is recommended:

```bash
piggyback_launch -ct -i bash
piggyback_launch -ct bash -c 'find / > allmyfiles'
```

## Safety and compatibility

We tested Piggybacking on Linux Ubuntu 16.04 LTS, and macOS X Sierra, with respectively bash versions 4.3, and 3.2. Piggybacking should be compatible with any POSIX system with a reasonably recent version of bash. Internally, it uses POSIX commands basename, chmod, dd, fold, grep, head, kill, mkdir, ps, rm, sed, sleep, tail, and tr; and POSIX environment variables HOME and USER. If any of those commands deviates from their POSIX specifications, Piggybacking may no longer work. Piggybacking uses the /dev/urandom device as a source of random bytes, and needs a GNU-like mktemp command; those resources are non-POSIX, but available in most *nixes.

Piggybacking works by granting access to certain files to everyone in the cluster, and as such it may pose security risks. **Calling piggyback_launch automatically grants +x access to your home for everyone**. A control directory ~/.piggybacking is created, with -rw+x access for everyone, and trigger files are created inside this directory, with +rw-x access for everyone.

Piggybacking is not suited for misson-critical environments, for large amounts of users, or for clusters where the nodes are publically acessible — it assumes at least one extra layer of security, like logging to a VPN or an SSH entry point before acessing the nodes. And, again, it comes with **no guarantees** --- use it at your own risk (see LICENSE).

## About us

The [RECOD Lab](https://recodbr.wordpress.com/) was founded at the [Institute of Computing](http://www.ic.unicamp.br/) of [UNICAMP](http://www.unicamp.br/) in 2010, to foster research on complex data: multimedia, social networks, spatio-temporal, just to mention a few. Computer Vision, Information Retrieval, Machine Learning, and Digital Forensics are some of the research interests of the group.

[Prof. Eduardo Valle](http://eduardovalle.com/) is a faculty member of RECOD.

## Copyright and license

Please check files LICENSE/AUTHORS, LICENSE/CONTRIBUTORS, and LICENSE/LICENSE.
