#  Update the way in which kubelet determines whether a container meets the given spec
Author: @Lichuqiang @wackxu


## Background & Motivations 

Currently kubelet hashes the container spec and store it with the container so that it can detect whether the container meets the current spec. If the hashes do not match, kubelet would kill the container and create a new one. The most notable side effect of this mechanism is that during upgrade, if a new field is added to the container spec in the 1.x kubernetes version, and kubelet is upgraded to 1.x from 1.x-1 version, all containers created by the 1.x-1 version kubelet will be killed, which users would not like to see.
Now that latest docker supports live-restore, we should consider fixing this in k8s.

For more details see issue https://github.com/kubernetes/kubernetes/issues/63814

## Proposal

Use serialized container spec to determines whether a container meets the given spec instead of container hash.

## Implementation



```go
func containerChanged(container *v1.Container, containerStatus *kubecontainer.ContainerStatus) (string, string, bool) {
	expectedSpec := kubecontainer.(container)

	oldContainer := &v1.Container{}
	if err := json.Unmarshal([]byte(containerStatus.SerializedSpec), oldContainer); err != nil {
		glog.Errorf("Unable to unmarshal serialized container spec %q: %v", container.Name, err)
		return expectedSpec, containerStatus.SerializedSpec, false
	}

	// apply defaults to container.
	api.SetDefaults_ContainerSpec(oldContainer)

	// do semantic deep equal to see if there was a diff that required restarting.
	isEqual := apiequality.Semantic.DeepEqual(container, oldContainer)

	return expectedSpec, containerStatus.SerializedSpec, isEqual
}
```

```
// SerializedContainerSpec returns the serialized spec of the container. It is used to compare
// the running container with its desired spec.
func SerializedContainerSpec(container *v1.Container) string {
	rawCon, err := json.Marshal(container)
	if err != nil {
		glog.Errorf("Unable to marshal container %q: %v", container.Name, err)
		return ""
	}
	return string(rawCon)
}
```
