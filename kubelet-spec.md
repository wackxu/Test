#  Update the way in which kubelet determines whether a container meets the given spec

## Background 

Currently kubelet hashes the container spec and store it with the container so that it can detect whether the container meets the current spec. If the hashes do not match, kubelet would kill the container and create a new one. The most notable side effect of this mechanism is that during upgrade, if a new field is added to the container spec in the 1.x kubernetes version, and kubelet is upgraded to 1.x from 1.x-1 version, all containers created by the 1.x-1 version kubelet will be killed, which users would not like to see.
Now that latest docker supports live-restore, we should consider fixing this in k8s.
