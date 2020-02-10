#ifndef PARSENMEA_H_211217
#define PARSENMEA_H_211217

#include <string>
#include <list>
#include <vector>
#include <utility>
#include <istream>

#include "position.h"

namespace NMEA
{
  /* Determine whether the parameter is a well-formed NMEA sentence.
   * A NMEA sentence contains the following contents:
   *   - the prefix "$GP";
   *   - followed by a sequence of three (English) alphabet characters identifying
   *     the sentence format;
   *   - followed by a sequence of comma-separated data fields;
   *   - followed by a '*' character;
   *   - followed by a two-character hexadecimal checksum.
   * The '$' and '*' characters are reserved, and may not appear in the data fields.
   *
   * For ill-formed sentences, this function returns false (it does not throw an
   * exception or terminate the program).
   */
  bool isWellFormedSentence(std::string);


  /* Verify whether a sentence has a valid checksum.
   * To be valid, the checksum value should equal the XOR reduction of the character
   * codes of all characters between the '$' and the '*' (exclusive).
   *
   * Pre-condition: the parameter is a well-formed NMEA sentence.
   */
  bool hasValidChecksum(std::string);


  /* The first component of the pair is a NMEA sentence format.
   * The second component is a vector of sentence fields, excluding the checksum.
   * The elements of the vector should not include the separating commas.
   * E.g. the first component of the pair could be "GLL", the first element of the
   * vector could be "5425.32", and the second element of the vector could be "N".
   */
  using SentenceData = std::pair<std::string, std::vector<std::string>>;


  /* Extracts the sentence format and the field contents from a NMEA sentence.
   * The '$GP' and the checksum are ignored.
   *
   * Pre-condition: the parameter is a well-formed NMEA sentence.
   */
  SentenceData extractSentenceData(std::string);


  /* Computes a Position from NMEA Sentence Data.
   * Currently only supports the GLL, GGA and RMC sentence formats.
   *
   * Throws a std::invalid_argument exception for unsupported sentence formats, or
   * if the neccessary data fields are missing or contain invalid data.
   */
  GPS::Position positionFromSentenceData(SentenceData);


  /* A route is a sequence of positions.
   */
  using Route = std::vector<GPS::Position>;


  /* Reads a stream of NMEA sentences (one sentence per line), and constructs a
   * Route, ignoring any lines that do not contain valid sentences.
   *
   * A sentence is valid if:
   *  - it is a well-formed NMEA sentence;
   *  - the checksum is valid;
   *  - the sentence format is supported (currently GLL, GGA and RMC);
   *  - the neccessary data fields are present and contain valid data.
   */
  Route routeFromLog(std::istream &);

}

#endif
