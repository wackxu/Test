func (cs *controllerServer) ListSnapshots(ctx context.Context, req *csi.ListSnapshotsRequest) (*csi.ListSnapshotsResponse, error) {
	if err := cs.Driver.ValidateControllerServiceRequest(csi.ControllerServiceCapability_RPC_LIST_SNAPSHOTS); err != nil {
		glog.V(3).Infof("invalid list snapshot req: %v", req)
		return nil, err
	}

	var snapshots []hostPathSnapshot

	// case 1: SnapshotId is not empty, return snapshots that match the snapshot id.
	if len(req.GetSnapshotId()) != 0 {
		snapshotID := req.SnapshotId
		if snapshot, ok := hostPathVolumeSnapshots[snapshotID]; ok {
			snapshots = append(snapshots, snapshot)
			// jump to the result processing.
			goto result
		}
	}

	// case 2: SourceVolumeId is not empty, return snapshots that match the source volume id.
	if len(req.SourceVolumeId) != 0 {
		for _, snapshot := range hostPathVolumeSnapshots {
			if snapshot.VolID == req.SourceVolumeId {
				snapshots = append(snapshots, snapshot)
				// jump to the result processing.
				goto result
			}
		}
	}

	// case 3: no parameter is set, so we return all the snapshots.
	for _, snapshot := range hostPathVolumeSnapshots {
		snapshots = append(snapshots, snapshot)
	}

result:
	var entries []*csi.ListSnapshotsResponse_Entry
	for _, snap := range snapshots {
		entries = append(entries, &csi.ListSnapshotsResponse_Entry{
			Snapshot: &csi.Snapshot{
				Id:             snap.Id,
				SourceVolumeId: snap.VolID,
				CreatedAt:      snap.CreateAt,
				Status:         snap.Status,
			},
		})
	}

	rsp := &csi.ListSnapshotsResponse{
		Entries: entries,
	}

	return rsp, nil
}
