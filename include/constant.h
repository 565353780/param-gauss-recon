#pragma once

#include <string>

const int CHUNK_SIZE = 512;
const float R_SQ_STOP_EPS = 1e-20;
const float TARGET_ISO_VALUE = -0.5;  // this is compatible with the MC orientation
const float EPSILON = 1e-6;

const std::string EXPORT_QUERY_EXE = "../../param-gauss-recon/bin/PGRExportQuery";
const std::string LOAD_QUERY_EXE = "../../param-gauss-recon/bin/PGRLoadQuery";
