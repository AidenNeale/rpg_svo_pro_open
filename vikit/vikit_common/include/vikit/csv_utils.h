#pragma once

// TODO(cfo): These functions are from Thomas Schneider's dataset_tools
//            We should move to that dataset provider as soon as it is ready!

#include <kindr/minimal/position.h>
#include <kindr/minimal/quat-transformation.h>
#include <kindr/minimal/rotation-quaternion.h>

#include <Eigen/Geometry>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace vk {

typedef kindr::minimal::QuatTransformation Transformation;
typedef kindr::minimal::RotationQuaternion Quaternion;
typedef kindr::minimal::AngleAxis AngleAxis;
typedef kindr::minimal::Position Position3D;

inline void removeAnyEndOfLineCharacter(std::string* line) {
  if (!line) {
    throw std::invalid_argument("removeAnyEndOfLineCharacter: line is null.");
  }
  for (std::string::iterator it = line->begin(); it != line->end();) {
    if ((*it == '\r') || (*it == '\n')) {
      it = line->erase(it);
    } else {
      ++it;
    }
  }
}

inline bool readNextLine(const size_t num_fields_per_line, std::ifstream& csv_file_stream,
                         std::vector<std::string>* fields) {
  if (!fields) {
    throw std::invalid_argument("readNextLine: fields is null.");
  }
  fields->clear();
  // Read the line.
  std::string line;
  if (!std::getline(csv_file_stream, line)) {
    return false;
  }

  // Treat empty lines as end-of-file (hopefully at the end of the file).
  removeAnyEndOfLineCharacter(&line);
  if (line.empty()) {
    return false;
  }

  // Split fields of the read line.
  const char field_delimiter = ',';
  fields->resize(num_fields_per_line);
  std::stringstream line_stream(line);
  for (size_t idx = 0u; idx < num_fields_per_line; ++idx) {
    if (line_stream.eof()) {
      throw std::runtime_error("readNextLine: Not enough fields in line. Expected " +
                               std::to_string(num_fields_per_line) + " fields but found only " +
                               std::to_string(idx) + ".");
    }
    std::getline(line_stream, (*fields)[idx], field_delimiter);

    if ((*fields)[idx].empty()) {
      throw std::runtime_error("readNextLine: Too few fields per line.");
    }
  }
  return true;
}

inline double convertStringToDouble(const std::string& number_as_string) {
  if (number_as_string.empty()) {
    throw std::invalid_argument("convertStringToDouble: number_as_string is empty.");
  }
  double number_as_double = 0;
  try {
    number_as_double = std::stod(number_as_string);
  } catch (const std::invalid_argument& invalid_argument_exception) {
    throw std::invalid_argument("Invalid argument exception. Could not convert '" +
                                number_as_string + "' to a double. " +
                                invalid_argument_exception.what());
  } catch (const std::out_of_range& out_of_range_exception) {
    throw std::out_of_range("Out of range exception. The given number '" + number_as_string +
                            "' does not seem to fit into a double! " +
                            out_of_range_exception.what());
  } catch (const std::exception& exception) {
    throw std::runtime_error("Conversion of the given number '" + number_as_string +
                             "' to a double failed. " + exception.what());
  }
  return number_as_double;
}

inline Eigen::Vector3d convertStringsToEigenVector3d(const std::string& string_x,
                                                     const std::string& string_y,
                                                     const std::string& string_z) {
  if (string_x.empty()) {
    throw std::invalid_argument("convertStringsToEigenVector3d: string_x is empty.");
  }
  if (string_y.empty()) {
    throw std::invalid_argument("convertStringsToEigenVector3d: string_y is empty.");
  }
  if (string_z.empty()) {
    throw std::invalid_argument("convertStringsToEigenVector3d: string_z is empty.");
  }
  Eigen::Vector3d vector;
  vector.setZero();
  try {
    vector << std::stod(string_x), std::stod(string_y), std::stod(string_z);
  } catch (const std::invalid_argument& invalid_argument_exception) {
    throw std::invalid_argument("Invalid argument exception. Could not convert '(" + string_x +
                                ", " + string_y + ", " + string_z + ")' to Eigen::Vector3d." +
                                invalid_argument_exception.what());
  } catch (const std::out_of_range& out_of_range_exception) {
    throw std::out_of_range("Out of range exception. The given tuple '(" + string_x + ", " +
                            string_y + ", " + string_z + ")' does not seem to fit into a " +
                            "three-dimensional double vector!" + out_of_range_exception.what());
  } catch (const std::exception& exception) {
    throw std::runtime_error("Conversion of the given tuple '(" + string_x + ", " + string_y +
                             ", " + string_z + ")' to three-dimensional double vector failed. " +
                             exception.what());
  }
  return vector;
}

inline Quaternion convertStringsToQuaternion(const std::string& qw, const std::string& qx,
                                             const std::string& qy, const std::string& qz) {
  if (qw.empty()) {
    throw std::invalid_argument("convertStringsToQuaternion: qw is empty.");
  }
  if (qx.empty()) {
    throw std::invalid_argument("convertStringsToQuaternion: qx is empty.");
  }
  if (qy.empty()) {
    throw std::invalid_argument("convertStringsToQuaternion: qy is empty.");
  }
  if (qz.empty()) {
    throw std::invalid_argument("convertStringsToQuaternion: qz is empty.");
  }

  Quaternion quaternion;
  quaternion.setIdentity();
  try {
    constexpr double kNormTolerance = 1e-3;
    Eigen::Vector4d quat_coeffs(std::stod(qw), std::stod(qx), std::stod(qy), std::stod(qz));
    if (std::fabs(1.0 - quat_coeffs.norm()) > kNormTolerance) {
      throw std::runtime_error("Invalid quaternion norm: " + std::to_string(quat_coeffs.norm()));
    }
    quat_coeffs.normalize();
    quaternion = Quaternion(quat_coeffs(0), quat_coeffs(1), quat_coeffs(2), quat_coeffs(3));
  } catch (const std::invalid_argument& invalid_argument_exception) {
    throw std::invalid_argument("Invalid argument exception. Could not convert '(" + qw + ", " +
                                qx + ", " + qy + ", " + qz + ")' to aslam::Quaternion." +
                                invalid_argument_exception.what());
  } catch (const std::out_of_range& out_of_range_exception) {
    throw std::out_of_range("Out of range exception. The given tuple '(" + qw + ", " + qx + ", " +
                            qy + ", " + qz + ")' does not seem to fit the definition " +
                            "of an aslam::Quaternion!" + out_of_range_exception.what());
  } catch (const std::exception& exception) {
    throw std::runtime_error("Conversion of the given tuple '(" + qw + ", " + qx + ", " + qy +
                             ", " + qz + ")' to aslam::Quaternion failed. " + exception.what());
  }
  return quaternion;
}

inline int64_t convertStringToLongLong(const std::string& number_as_string) {
  if (number_as_string.empty()) {
    throw std::invalid_argument("convertStringToLongLong: number_as_string is empty.");
  }
  int64_t number_as_longlong = 0;
  try {
    number_as_longlong = std::stoll(number_as_string);
  } catch (const std::invalid_argument& invalid_argument_exception) {
    throw std::invalid_argument("Invalid argument exception. Could not convert '" +
                                number_as_string + "' to a longlong. " +
                                invalid_argument_exception.what());
  } catch (const std::out_of_range& out_of_range_exception) {
    throw std::out_of_range("Out of range exception. The given number '" + number_as_string +
                            "' does not seem to fit into a longlong! " +
                            out_of_range_exception.what());
  } catch (const std::exception& exception) {
    throw std::runtime_error("Conversion of the given number '" + number_as_string +
                             "' to a longlong failed. " + exception.what());
  }
  return number_as_longlong;
}

}  // namespace vk
