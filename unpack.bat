set ENV_NAME=conda-amcax-win

if not exist .\3rd\%ENV_NAME% (
    mkdir .\3rd\%ENV_NAME%
)

tar -xvzf .\3rd\%ENV_NAME%.tar.gz -C .\3rd\%ENV_NAME%

cd .\3rd\%ENV_NAME%

.\bin\conda-unpack
