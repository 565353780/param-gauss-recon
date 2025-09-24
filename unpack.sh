ENV_NAME='conda-amcax'

mkdir -p ./output/${ENV_NAME}/

tar -xvzf ./output/${ENV_NAME}.tar.gz -C ./output/${ENV_NAME}/

cd ./output/${ENV_NAME}
./bin/conda-unpack
