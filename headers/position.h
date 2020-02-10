#ifndef POSITION_H_211217
#define POSITION_H_211217

#include <string>

#include "types.h"

namespace GPS
{
  class Position
  {
    public:

      /* Construct a Position from degrees latitude, degrees longitude, and
       * (optionally) elevation in metres.
       */
      Position(degrees lat, degrees lon, metres ele = 0.0);


      /* Construct a Position from strings containing a decimal degrees
       * representation of latitude and longitude, and (optionally) elevation in
       * metres.
       */
      Position(std::string latStr,
               std::string lonStr,
               std::string eleStr = "0");


      /* Construct a Position from strings containing a positive DDM (degrees and
       * decimal minutes) representation of latitude and longitude, along with
       * 'N'/'S' and 'E'/'W' characters to indicate positive or negative angles,
       * and (optionally) elevation in metres.
       */
      Position(std::string ddmLatStr, char northing,
               std::string ddmLonStr, char easting,
               std::string eleSt = "0");

      degrees latitude() const;
      degrees longitude() const;
      metres  elevation() const;

      std::string toString(bool includeElevation = true) const;

      /* Computes an approximation of the distance between two Positions on the Earth's surface.
       * Does not take into account elevation.
       */
      static metres distanceBetween(Position, Position);

    private:
      degrees lat;
      degrees lon;
      metres  ele;
  };


  /* Convert a DDM (degrees and decimal minutes) string representation of an angle to a
     DD (decimal degrees) value.
   */
  degrees ddmTodd(std::string);
}

#endif
