ENV_NAME='conda-amcax'

mkdir ./output

conda pack -n amcax -o ./output/${ENV_NAME}.tar.gz
