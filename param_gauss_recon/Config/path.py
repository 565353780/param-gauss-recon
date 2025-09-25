import platform


EXPORT_QUERY_EXE = "../param-gauss-recon/bin/PGRExportQuery"
LOAD_QUERY_EXE = "../param-gauss-recon/bin/PGRLoadQuery"

if platform.system() == "Windows":
    EXPORT_QUERY_EXE += ".exe"
    LOAD_QUERY_EXE += ".exe"
