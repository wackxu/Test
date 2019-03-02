wget https://github.com/apache/incubator-mxnet/releases/download/1.3.1/apache-mxnet-src-1.3.1.rc0-incubating.tar.gz --no-check-certificate


python train_imagenet.py --benchmark 1 --network vgg --batch-size 64 --image-shape 3,299,299 --num-epochs 1 --kv-store device --num-examples 3200 --num-layers 11


docker login -u cn-north-1@sFudQEkiUF3cwPWar6I2 -p c405fd33e625d7b5f5880adfd24e54a7805ea8850e4c7ef492a40d831649e1e9 swr.cn-north-1.myhuaweicloud.com

$ sudo docker tag [{镜像名称}:{版本名称}] swr.cn-north-1.myhuaweicloud.com/{组织名称}/{镜像名称}:{版本名称}

$ sudo docker push swr.cn-north-1.myhuaweicloud.com/{组织名称}/{镜像名称}:{版本名称}
