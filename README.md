wget https://github.com/apache/incubator-mxnet/releases/download/1.3.1/apache-mxnet-src-1.3.1.rc0-incubating.tar.gz --no-check-certificate


python train_imagenet.py --benchmark 1 --network vgg --batch-size 64 --image-shape 3,299,299 --num-epochs 1 --kv-store device --num-examples 3200 --num-layers 11
