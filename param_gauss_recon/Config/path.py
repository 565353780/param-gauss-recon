import platform


EXPORT_QUERY_EXE = "../param-gauss-recon/bin/PGRExportQuery"
LOAD_QUERY_EXE = "../param-gauss-recon/bin/PGRLoadQuery"

if platform.system() == "Windows":
    EXPORT_QUERY_EXE = EXPORT_QUERY_EXE.replace("/", "\\") + ".exe"
    LOAD_QUERY_EXE = LOAD_QUERY_EXE.replace("/", "\\") + ".exe"
