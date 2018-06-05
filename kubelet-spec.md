#  Update the way in which kubelet determines whether a container meets the given spec
Author: @Lichuqiang @wackxu


## Background & Motivations 

Currently kubelet hashes the container spec and store it with the container so that it can detect whether the container meets the current spec. If the hashes do not match, kubelet would kill the container and create a new one. The most notable side effect of this mechanism is that during upgrade, if a new field is added to the container spec in the 1.x kubernetes version, and kubelet is upgraded to 1.x from 1.x-1 version, all containers created by the 1.x-1 version kubelet will be killed, which users would not like to see.
Now that latest docker supports live-restore, we should consider fixing this in k8s.

For more details see issue  

https://github.com/kubernetes/kubernetes/issues/63814  
https://github.com/kubernetes/kubernetes/issues/23104

## Proposal

Use serialized container spec to determines whether a container meets the given spec instead of container hash.

## Design Overview

Instead of store hash values in container labels, we can save the serialized container spec we had when we launched the container. And during detection, kubelet can load it, apply defaults, and do a semantic deepequal to see if there was an actual diff that required restarting.

## Implementation

1. Adds new label `containerSerializedSpecLabel`, to save serialized container spec; Load and apply the defaults for deep equal.

```go
containerSerializedSpecLabel = "io.kubernetes.container.spec"
```
2. caclute the serialized container spec.

```go
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
3. detect the running container whether meet the desired spec.

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
For older container without `containerSerializedSpecLabel` in labels, we will force add the label to the container and the container will restart, because that the container labels cannot be updated (moby/moby#21721) without a restart currently.  

## Alternatives considered

### Hash certain fields instead of whole container spec
Instead of hash the whole container spec, we can maintain a list of container fields of different releases that should be taken into consideration during hash, and identify the hash values with release prefix.
A field list may looks like:

```
release1: {field1, field2...fieldN}
release2: {field1, field2...fieldN, fieldN+1}
```

and imagine the kubelet has been upgraded to release2 (new container field fieldN+1 added), when walking through the pods, if it found a container has a hash with a prefix of "release1", then it knows the container should be hashed with the fields of release1, thus, the result hash would not change.

A downside is that we might need to maintain the field list manually (e.g deprecate the field of old releases).
And also, if a user changes newly added container fields of a static pod that is launched in old way, he will not see the pods restart as expect, instead, he’ll have to delete and recreate it.

### Add release labels to container fields
Instead of hardcode the fields that need hash in a list, we can also consider to add labels in container fields API, to imply that the fields should be included in hash of certain release.
For example:

```go
type Container struct {
    foo string `containerSerializedSpecLabel:"since v1.9"`
    bar string `containerSerializedSpecLabel:"since v1.10"`
}
```

Then, “foo” will be included for checking container hash like "v1.9-xxxxxxxx"
“foo” and “bar” will be included for checking container hash like "v1.10-xxxxxxxx"

But the approach requires changes on API mechanism, which lots of people may not want to see.

## Backwards compatible
For all of the approaches above, for the first time a kubelet is upgraded to a version with the change introduced, containers launched in old ways will be restart, because all of the three approaches change the way we save/apply container identity.

If we do not expect to see this, we’ll need extra mechanism to ensure backwards compatible.
For example, we can keep old container labels as is, and add new labels to apply newly calculated identity values. When kubelet finds a container is labeled in the old way, it’ll skip it. Downside is that we’ll have to recreate pods manually if we want to update a static pod that is labeled in the old way.

For the 2nd and 3rd approach, we can also consider mark all of existing fields in container as in need of hash (instead of only those mutable fields), thus, hash values would not change upon upgrades.
