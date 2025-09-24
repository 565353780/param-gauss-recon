ENV_NAME='conda-amcax'

mkdir -p ./3rd/${ENV_NAME}/

tar -xvzf ./3rd/${ENV_NAME}.tar.gz -C ./3rd/${ENV_NAME}/

cd ./3rd/${ENV_NAME}
./bin/conda-unpack
