# egSC

SuperCollider Quark with tools and UGens for Egregious live performance.

This Quark can be checked out like any Quark in SuperCollider, as:

```
Quarks.install("egSC");
```

But also contains some C++ code, including UGens. To build and install the UGens you will first need to also clone the
submodules, so change to the Quark directory and run:

```
$ git submodule init
$ git submodule update
```

Then the process is very similar to that of [sc3-plugins](https://github.com/supercollider/sc3-plugins). Create a build/
directory, run ```cmake``` there with path to SuperCollider sources provided, then ```make install```. Right now only
building on Linux is supported.

