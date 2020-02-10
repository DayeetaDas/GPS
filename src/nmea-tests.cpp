#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ParseNMEATests
#include <boost/test/unit_test.hpp>

#include <string>
#include <stdexcept>
#include <vector>
#include <utility>
#include <ostream>
#include <fstream>
#include <sstream>
#include <iostream>

#include "logs.h"
#include "parseNMEA.h"

using namespace GPS;
using namespace NMEA;

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( IsWellFormedSentence )

BOOST_AUTO_TEST_CASE( WellFormedNoFields )
{
    BOOST_CHECK( isWellFormedSentence("$GPXXX*01") );
}

BOOST_AUTO_TEST_CASE( WellFormedOneField )
{
    BOOST_CHECK( isWellFormedSentence("$GPXXX,1*23") );
}

BOOST_AUTO_TEST_CASE( WellFormedTwoFields )
{
    BOOST_CHECK( isWellFormedSentence("$GPXXX,1,testing*69") );
}

BOOST_AUTO_TEST_CASE( WellFormedEmptyField )
{
    BOOST_CHECK( isWellFormedSentence("$GPXXX,*99") );
}

BOOST_AUTO_TEST_CASE( WellFormedManyFields )
{
    std::string commas(1000,','); // 1000 fields
    BOOST_CHECK( isWellFormedSentence("$GPXXX" + commas + "*88") );
}

BOOST_AUTO_TEST_CASE( WellFormedWithLowercaseHexCharacters )
{
    BOOST_CHECK( isWellFormedSentence("$GPXXX*af") );
    BOOST_CHECK( isWellFormedSentence("$GPXXX*3b") );
    BOOST_CHECK( isWellFormedSentence("$GPXXX*c5") );
}

BOOST_AUTO_TEST_CASE( WellFormedWithUppercaseHexCharacters )
{
    BOOST_CHECK( isWellFormedSentence("$GPXXX*DC") );
    BOOST_CHECK( isWellFormedSentence("$GPXXX*7E") );
    BOOST_CHECK( isWellFormedSentence("$GPXXX*A8") );
}

BOOST_AUTO_TEST_CASE( WellFormedTypicalSentences )
{
    BOOST_CHECK( isWellFormedSentence("$GPGLL,5425.31,N,107.03,W,82610*69") );
    BOOST_CHECK( isWellFormedSentence("$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,4.0,M,,M,,*40") );
    BOOST_CHECK( isWellFormedSentence("$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*62") );
    BOOST_CHECK( isWellFormedSentence("$GPMSS,55,27,318.0,100,*66") );
}

BOOST_AUTO_TEST_CASE( IllFormedMissingSuffix )
{
    BOOST_CHECK( ! isWellFormedSentence("") );
    BOOST_CHECK( ! isWellFormedSentence("$") );
    BOOST_CHECK( ! isWellFormedSentence("$G") );
    BOOST_CHECK( ! isWellFormedSentence("$GP") );
    BOOST_CHECK( ! isWellFormedSentence("$GPG") );
    BOOST_CHECK( ! isWellFormedSentence("$GPGL") );
    BOOST_CHECK( ! isWellFormedSentence("$GPGLL") );
    BOOST_CHECK( ! isWellFormedSentence("$GPGLL,") );
    BOOST_CHECK( ! isWellFormedSentence("$GPGLL*") );
    BOOST_CHECK( ! isWellFormedSentence("$GPGLL,*") );
    BOOST_CHECK( ! isWellFormedSentence("$GPGLL*1") );
    BOOST_CHECK( ! isWellFormedSentence("$GPGLL,*2") );
}

BOOST_AUTO_TEST_CASE( IllFormedDollar )
{
    BOOST_CHECK( ! isWellFormedSentence("SGPXXX*01") );
    BOOST_CHECK( ! isWellFormedSentence( "GPXXX*01") );
}

BOOST_AUTO_TEST_CASE( IllFormedGP )
{
    BOOST_CHECK( ! isWellFormedSentence("$HPXXX*01") );
    BOOST_CHECK( ! isWellFormedSentence("$PXXX*01") );
    BOOST_CHECK( ! isWellFormedSentence("$GQXXX*01") );
    BOOST_CHECK( ! isWellFormedSentence("$GXXX*01") );
    BOOST_CHECK( ! isWellFormedSentence("$KLXXX*01") );
    BOOST_CHECK( ! isWellFormedSentence("$XXX*01") );
}

BOOST_AUTO_TEST_CASE( IllFormedType )
{
    BOOST_CHECK( ! isWellFormedSentence("$GP*01") );
    BOOST_CHECK( ! isWellFormedSentence("$GPX*01") );
    BOOST_CHECK( ! isWellFormedSentence("$GPXX*01") );
    BOOST_CHECK( ! isWellFormedSentence("$GP1XX*01") );
    BOOST_CHECK( ! isWellFormedSentence("$GPX%X*01") );
    BOOST_CHECK( ! isWellFormedSentence("$GPXX *01") );
}

BOOST_AUTO_TEST_CASE( IllFormedReservedCharInField )
{
    BOOST_CHECK( ! isWellFormedSentence("$GPXXX,$77*01") );
    BOOST_CHECK( ! isWellFormedSentence("$GPXXX,2*3,1*77") );
}

BOOST_AUTO_TEST_CASE( IllFormedStar )
{
    BOOST_CHECK( ! isWellFormedSentence("$GPXXX77") );
    BOOST_CHECK( ! isWellFormedSentence("$GPXXX%77") );
}

BOOST_AUTO_TEST_CASE( IllFormedChecksum )
{
    BOOST_CHECK( ! isWellFormedSentence("$GPXXX*012") );
    BOOST_CHECK( ! isWellFormedSentence("$GPXXX*3g") );
    BOOST_CHECK( ! isWellFormedSentence("$GPXXX*h2") );
    BOOST_CHECK( ! isWellFormedSentence("$GPXXX*JL") );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( HasValidChecksum )

BOOST_AUTO_TEST_CASE( ValidChecksumMinimalSentence )
{
    BOOST_CHECK( hasValidChecksum("$GPAAA*56") );
}

BOOST_AUTO_TEST_CASE( IncorrectChecksumMinimalSentence )
{
    BOOST_CHECK( ! hasValidChecksum("$GPAAA*55") );
}

BOOST_AUTO_TEST_CASE( ValidChecksumTypicalSentences )
{
    BOOST_CHECK( hasValidChecksum("$GPGLL,5425.31,N,107.03,W,82610*69") );
    BOOST_CHECK( hasValidChecksum("$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,4.0,M,,M,,*40") );
    BOOST_CHECK( hasValidChecksum("$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*62") );
}

BOOST_AUTO_TEST_CASE( IncorrectChecksumTypicalSentences )
{
    BOOST_CHECK( ! hasValidChecksum("$GPGLL,5425.31,N,107.03,W,82610*24") );
    BOOST_CHECK( ! hasValidChecksum("$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,4.0,M,,M,,*41") );
    BOOST_CHECK( ! hasValidChecksum("$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*97") );
}

BOOST_AUTO_TEST_CASE( CorrectChecksumWithUppercaseHexDigit )
{
    BOOST_CHECK( hasValidChecksum("$GPXYZ*4C") );
}

BOOST_AUTO_TEST_CASE( CorrectChecksumWithLowercaseHexDigit )
{
    BOOST_CHECK( hasValidChecksum("$GPXYZ*4c") );
}

BOOST_AUTO_TEST_CASE( IncorrectChecksumsithUppercaseHexDigit )
{
    BOOST_CHECK( ! hasValidChecksum("$GPAAE*5F") );
}

BOOST_AUTO_TEST_CASE( IncorrectChecksumWithLowercaseHexDigit )
{
    BOOST_CHECK( ! hasValidChecksum("$GPAAE*5f") );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( ExtractSentenceData )

/////////////////////////////////////////////////////////////////////////

// Displaying vectors directly using the built-in BOOST macros is tricky.
// For reasons I don't fully understand, just overloading operator<< here doesn't work.
// See: https://stackoverflow.com/questions/17572583/boost-check-fails-to-compile-operator-for-custom-types
//
// So instead we introduce an auxilliary function for formatting the error message for mismatched vectors.

std::ostream& operator<<(std::ostream& outputStream, const std::vector<std::string> & vec)
{
    outputStream << '{';
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) outputStream << ',';
        outputStream << *it;
    }
    outputStream << '}';
    return outputStream;
}

std::string formatMismatchedFieldData(const std::vector<std::string> & actualFields, const std::vector<std::string> & expectedFields)
{
    std::stringstream outputMessage;
    outputMessage << "extractSentenceData has failed [ " << actualFields << " != " << expectedFields << " ]";
    return outputMessage.str();
}

void checkSentenceDataEqual(const SentenceData & actual, const SentenceData & expected)
{
    BOOST_CHECK_EQUAL( actual.first , expected.first );

    BOOST_CHECK_MESSAGE(actual.second == expected.second, formatMismatchedFieldData(actual.second,expected.second));
}

////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( ExtractZeroFields )
{
    SentenceData actualSentenceData = extractSentenceData("$GPAAA*56");
    SentenceData expectedSentenceData = { "AAA" , {} };

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( ExtractOneField )
{
    SentenceData actualSentenceData = extractSentenceData("$GPAAA,1*4b");
    SentenceData expectedSentenceData = { "AAA" , {"1"} };

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( ExtractTwoFields )
{
    SentenceData actualSentenceData = extractSentenceData("$GPAAA,1,testing*11");
    SentenceData expectedSentenceData = { "AAA" , {"1","testing"} };

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( ExtractGLL )
{
    SentenceData actualSentenceData = extractSentenceData("$GPGLL,5425.31,N,107.03,W,82610*69");
    SentenceData expectedSentenceData = { "GLL" , {"5425.31","N","107.03","W","82610"} };

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( ExtractGGA )
{
    SentenceData actualSentenceData = extractSentenceData("$GPGGA,114530.000,3722.6279,N,00559.1566,W,1,0,,1.0,M,,M,,*4E");
    SentenceData expectedSentenceData = { "GGA" , {"114530.000","3722.6279","N","00559.1566","W","1","0","","1.0","M","","M","",""} };

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( ExtractRMC )
{
    SentenceData actualSentenceData = extractSentenceData("$GPRMC,115856.000,A,3722.6710,N,00559.3014,W,0.000,0.00,150914,,A*6d");
    SentenceData expectedSentenceData = { "RMC" , {"115856.000","A","3722.6710","N","00559.3014","W","0.000","0.00","150914","","A"} };

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_CASE( ExtractMSS )
{
    SentenceData actualSentenceData = extractSentenceData("$GPMSS,55,27,318.0,100,*66");
    SentenceData expectedSentenceData = { "MSS" , {"55","27","318.0","100",""} };

    checkSentenceDataEqual(actualSentenceData , expectedSentenceData);
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( PositionFromSentenceData )

const double epsilon = 0.0001;
const double percentageAccuracy = 0.0001;

BOOST_AUTO_TEST_CASE( GLL_NW )
{
    SentenceData sentenceData = { "GLL", {"5425.31","N","107.03","W","82610"} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , ddmTodd("5425.31") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , -ddmTodd("107.03") , percentageAccuracy );
    BOOST_CHECK_SMALL( pos.elevation() , epsilon );
}

BOOST_AUTO_TEST_CASE( GLL_NE )
{
    SentenceData sentenceData = { "GLL", {"5425.31","N","107.03","E","82610"} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , ddmTodd("5425.31") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , ddmTodd("107.03") , percentageAccuracy );
    BOOST_CHECK_SMALL( pos.elevation() , epsilon );
}

BOOST_AUTO_TEST_CASE( GLL_SE )
{
    SentenceData sentenceData = { "GLL", {"5425.31","S","107.03","E","82610"} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , -ddmTodd("5425.31") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , ddmTodd("107.03") , percentageAccuracy );
    BOOST_CHECK_SMALL( pos.elevation() , epsilon );
}

BOOST_AUTO_TEST_CASE( GLL_SW )
{
    SentenceData sentenceData = { "GLL", {"5425.31","S","107.03","W","82610"} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , -ddmTodd("5425.31") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , -ddmTodd("107.03") , percentageAccuracy );
    BOOST_CHECK_SMALL( pos.elevation() , epsilon );
}

BOOST_AUTO_TEST_CASE( RMC_NW )
{
    SentenceData sentenceData = { "RMC", {"115856.000","A","3722.6710","N","00559.3014","W","0.000","0.00","150914","","A"} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , ddmTodd("3722.6710") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , -ddmTodd("00559.3014") , percentageAccuracy );
    BOOST_CHECK_SMALL( pos.elevation() , epsilon );
}

BOOST_AUTO_TEST_CASE( RMC_NE )
{
    SentenceData sentenceData = { "RMC", {"115856.000","A","3722.6710","N","00559.3014","E","0.000","0.00","150914","","A"} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , ddmTodd("3722.6710") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , ddmTodd("00559.3014") , percentageAccuracy );
    BOOST_CHECK_SMALL( pos.elevation() , epsilon );
}

BOOST_AUTO_TEST_CASE( RMC_SE )
{
    SentenceData sentenceData = { "RMC", {"115856.000","A","3722.6710","S","00559.3014","E","0.000","0.00","150914","","A"} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , -ddmTodd("3722.6710") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , ddmTodd("00559.3014") , percentageAccuracy );
    BOOST_CHECK_SMALL( pos.elevation() , epsilon );
}

BOOST_AUTO_TEST_CASE( RMC_SW )
{
    SentenceData sentenceData = { "RMC", {"115856.000","A","3722.6710","S","00559.3014","W","0.000","0.00","150914","","A"} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , -ddmTodd("3722.6710") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , -ddmTodd("00559.3014") , percentageAccuracy );
    BOOST_CHECK_SMALL( pos.elevation() , epsilon );
}

BOOST_AUTO_TEST_CASE( GGA_NW )
{
    SentenceData sentenceData = { "GGA", {"170834","4124.8963","N","08151.6838","W","1","05","1.5","280.2","M","-34.0","M","",""} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , ddmTodd("4124.8963") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , -ddmTodd("08151.6838") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.elevation() , 280.2 , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GGA_NE )
{
    SentenceData sentenceData = { "GGA", {"170834","4124.8963","N","08151.6838","E","1","05","1.5","280.2","M","-34.0","M","",""} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , ddmTodd("4124.8963") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , ddmTodd("08151.6838") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.elevation() , 280.2 , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GGA_SE )
{
    SentenceData sentenceData = { "GGA", {"170834","4124.8963","S","08151.6838","E","1","05","1.5","280.2","M","-34.0","M","",""} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , -ddmTodd("4124.8963") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , ddmTodd("08151.6838") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.elevation() , 280.2 , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GGA_SW )
{
    SentenceData sentenceData = { "GGA", {"170834","4124.8963","S","08151.6838","W","1","05","1.5","280.2","M","-34.0","M","",""} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , -ddmTodd("4124.8963") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , -ddmTodd("08151.6838") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.elevation() , 280.2 , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( GGA_NegativeElevation )
{
    SentenceData sentenceData = { "GGA", {"170834","4124.8963","S","08151.6838","W","1","05","1.5","-280.2","M","-34.0","M","",""} };
    Position pos = positionFromSentenceData(sentenceData);
    BOOST_CHECK_CLOSE( pos.latitude() , -ddmTodd("4124.8963") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.longitude() , -ddmTodd("08151.6838") , percentageAccuracy );
    BOOST_CHECK_CLOSE( pos.elevation() , -280.2 , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( UnsupportedFormat )
{
    SentenceData sentenceData = { "MSS", {"55","27","318.0","100",""} };
    BOOST_CHECK_THROW( positionFromSentenceData(sentenceData) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE( EmptyFieldVector )
{
    SentenceData sentenceDataGLL = { "GLL", {} };
    BOOST_CHECK_THROW( positionFromSentenceData(sentenceDataGLL) , std::invalid_argument );

    SentenceData sentenceDataRMC = { "RMC", {} };
    BOOST_CHECK_THROW( positionFromSentenceData(sentenceDataRMC) , std::invalid_argument );

    SentenceData sentenceDataGGA = { "GGA", {} };
    BOOST_CHECK_THROW( positionFromSentenceData(sentenceDataGGA) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE( MissingFieldsGLL )
{
    SentenceData missingN = { "GLL", {"5425.31","107.03","E","82610"} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingN) , std::invalid_argument );

    SentenceData missingE = { "GLL", {"5425.31","N","107.03","82610"} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingE) , std::invalid_argument );

    SentenceData missingLat = { "GLL", {"N","107.03","E","82610"} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingLat) , std::invalid_argument );

    SentenceData missingLon = { "GLL", {"5425.31","N","E","82610"} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingLon) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE( MissingFieldsRMC )
{
    SentenceData missingN = { "RMC", {"115856.000","A","3722.6710","00559.3014","E","0.000","0.00","150914","","A"} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingN) , std::invalid_argument );

    SentenceData missingE = { "RMC", {"115856.000","A","3722.6710","S","00559.3014","0.000","0.00","150914","","A"} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingE) , std::invalid_argument );

    SentenceData missingLat = { "RMC", {"115856.000","A","S","00559.3014","E","0.000","0.00","150914","","A"} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingLat) , std::invalid_argument );

    SentenceData missingLon = { "RMC", {"115856.000","A","3722.6710","S","E","0.000","0.00","150914","","A"} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingLon) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE( MissingFieldsGGA )
{
    SentenceData missingN = { "GGA", {"170834","4124.8963","08151.6838","W","1","05","1.5","280.2","M","-34.0","M","",""} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingN) , std::invalid_argument );

    SentenceData missingW = { "GGA", {"170834","4124.8963","N","08151.6838","1","05","1.5","280.2","M","-34.0","M","",""} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingW) , std::invalid_argument );

    SentenceData missingM = { "GGA", {"170834","4124.8963","N","08151.6838","W","1","05","1.5","",""} };
    BOOST_CHECK_THROW( positionFromSentenceData(missingM) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE( InvalidFieldData )
{
    SentenceData invalidGLL_N = { "GLL", {"three","N","107.03","W","82610"} };
    BOOST_CHECK_THROW( positionFromSentenceData(invalidGLL_N) , std::invalid_argument );

    SentenceData invalidRMC_W = { "RMC", {"115856.000","A","3722.6710","N","?&*","W","0.000","0.00","150914","","A"} };
    BOOST_CHECK_THROW( positionFromSentenceData(invalidRMC_W) , std::invalid_argument );

    SentenceData invalidGGA_M = { "GGA", {"170834","4124.8963","N","08151.6838","W","1","05","1.5","zero","M","-34.0","M","",""} };
    BOOST_CHECK_THROW( positionFromSentenceData(invalidGGA_M) , std::invalid_argument );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( RouteFromLog )

const double percentageAccuracy = 0.0001;

const std::string validGLLSentence = "$GPGLL,5425.31,N,107.03,W,82610*69";
const std::string validRMCSentence = "$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*62";
const std::string validMSSSentence = "$GPMSS,55,27,318.0,100,*66";

const GPS::Position gllPos = GPS::Position("5425.31",'N',"107.03",'W');
const GPS::Position rmcPos = GPS::Position("3722.5993",'N',"00559.2458",'W');

BOOST_AUTO_TEST_CASE( EmptyLog )
{
    std::stringstream log("");
    Route route = routeFromLog(log);

    BOOST_CHECK_EQUAL( route.size() , 0 );
}

BOOST_AUTO_TEST_CASE( LogWithOneValidSentence )
{
    std::stringstream log;
    log << validGLLSentence << std::endl;
    Route route = routeFromLog(log);

    BOOST_REQUIRE_EQUAL( route.size() , 1 );
    BOOST_CHECK_CLOSE( route[0].latitude() , gllPos.latitude() , percentageAccuracy );
    BOOST_CHECK_CLOSE( route[0].longitude() , gllPos.longitude(), percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( LogWithTwoValidSentences )
{
    std::stringstream log;
    log << validGLLSentence << std::endl;
    log << validRMCSentence << std::endl;
    Route route = routeFromLog(log);

    BOOST_REQUIRE_EQUAL( route.size() , 2 );

    BOOST_CHECK_CLOSE( route[0].latitude() , gllPos.latitude() , percentageAccuracy );
    BOOST_CHECK_CLOSE( route[0].longitude() , gllPos.longitude() , percentageAccuracy );

    BOOST_CHECK_CLOSE( route[1].latitude() , rmcPos.latitude() , percentageAccuracy );
    BOOST_CHECK_CLOSE( route[1].longitude() , rmcPos.longitude() , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( LogWithBlankLines )
{
    std::stringstream log;
    log << std::endl << std::endl;
    log << validGLLSentence << std::endl;
    log << std::endl;
    log << validRMCSentence << std::endl;
    log << std::endl << std::endl << std::endl;
    Route route = routeFromLog(log);

    BOOST_CHECK_EQUAL( route.size() , 2 );
}

BOOST_AUTO_TEST_CASE( LogWithoutLineBreaks )
{
    std::stringstream log;
    log << validGLLSentence << validRMCSentence << std::endl;
    Route route = routeFromLog(log);

    BOOST_CHECK_EQUAL( route.size() , 0 );
}

BOOST_AUTO_TEST_CASE( LogWithIllFormedSentences )
{
    std::stringstream log;
    log << "Arbitrary meta-data" << std::endl;
    log << validGLLSentence << std::endl;
    log << "$XXX*01" << std::endl;
    log << validRMCSentence << std::endl;
    log << "$GPGLL" << std::endl;
    Route route = routeFromLog(log);

    BOOST_CHECK_EQUAL( route.size() , 2 );
}

BOOST_AUTO_TEST_CASE( LogWithInvalidChecksums )
{
    std::stringstream log;
    log << "$GPGLL,5425.31,N,107.03,W,82610*24" << std::endl;
    log << validGLLSentence << std::endl;
    log << "$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,4.0,M,,M,,*41" << std::endl;
    log << validRMCSentence << std::endl;
    log << "$GPRMC,113922.000,A,3722.5993,N,00559.2458,W,0.000,0.00,150914,,A*97" << std::endl;
    Route route = routeFromLog(log);

    BOOST_CHECK_EQUAL( route.size() , 2 );
}

BOOST_AUTO_TEST_CASE( LogWithUnsupportedSentenceTypes )
{
    std::stringstream log;
    log << validGLLSentence << std::endl;
    log << validMSSSentence << std::endl; // unsupported
    log << validRMCSentence << std::endl;
    Route route = routeFromLog(log);

    BOOST_CHECK_EQUAL( route.size() , 2 );
}

BOOST_AUTO_TEST_CASE( LogWithMissingFields )
{
    std::string gllSentenceWithMissingFields = "$GPGLL,5425.31,107.03,W,82610*0B";
    std::string rmcSentenceWithMissingFields = "$GPRMC,113922.000,A,3722.5993,N,00559.2458*4C";
    std::string ggaSentenceWithMissingFields = "$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0*46";

    std::stringstream log;
    log << gllSentenceWithMissingFields << std::endl;
    log << validGLLSentence << std::endl;
    log << rmcSentenceWithMissingFields << std::endl;
    log << validRMCSentence << std::endl;
    log << ggaSentenceWithMissingFields << std::endl;
    Route route = routeFromLog(log);

    BOOST_CHECK_EQUAL( route.size() , 2 );
}

BOOST_AUTO_TEST_CASE( LogWithInvalidFields )
{
    std::stringstream log;
    log << "$GPGLL,5425.31,X,107.03,W,82610*7F" << std::endl;
    log << "$GPGLL,fivethousand,N,107.03,W,82610*41" << std::endl;
    log << validGLLSentence << std::endl;
    log << "$GPRMC,113922.000,A,3722.5993,N,00559.2458,7,0.000,0.00,150914,,A*02" << std::endl;
    log << "$GPRMC,113922.000,A,3722.5993,N,???,W,0.000,0.00,150914,,A*41" << std::endl;
    log << validRMCSentence << std::endl;
    log << "$GPGGA,113922.000,3722.5993,N,00559.2458,W,1,0,,high,M,,M,,*64" << std::endl;
    Route route = routeFromLog(log);

    BOOST_CHECK_EQUAL( route.size() , 2 );
}

BOOST_AUTO_TEST_CASE( LargeLog_GLL )
{
    std::fstream log(LogFiles::NMEALogsDir + "gll.log");
    Route route = routeFromLog(log);

    BOOST_REQUIRE_EQUAL( route.size() , 1090 );

    // $GPGLL,5425.32,N,107.11,W,82319*65
    BOOST_CHECK_CLOSE( route[0].latitude() , ddmTodd("5425.32") , percentageAccuracy );
    BOOST_CHECK_CLOSE( route[0].longitude() , -ddmTodd("107.11") , percentageAccuracy );

    // $GPGLL,5430.32,N,106.39,W,154912*51
    BOOST_CHECK_CLOSE( route[1000].latitude() , ddmTodd("5430.32") , percentageAccuracy );
    BOOST_CHECK_CLOSE( route[1000].longitude() , -ddmTodd("106.39") , percentageAccuracy );
}

BOOST_AUTO_TEST_CASE( LargeLog_GGA_RMC )
{
    std::fstream log(LogFiles::NMEALogsDir + "gga_rmc-1.log");
    Route route = routeFromLog(log);

    BOOST_REQUIRE_EQUAL( route.size() , 632 );  // The header and blank line should be discarded

    // $GPGGA,094627.000,3723.1622,N,00559.5788,W,1,0,,30.0,M,,M,,*7A
    BOOST_CHECK_CLOSE( route[0].latitude() , ddmTodd("3723.1622") , percentageAccuracy );
    BOOST_CHECK_CLOSE( route[0].longitude() , -ddmTodd("00559.5788") , percentageAccuracy );
    BOOST_CHECK_CLOSE( route[0].elevation() , 30 , percentageAccuracy );

    // $GPRMC,113720.000,A,3722.5563,N,00559.2403,W,0.000,0.00,150914,,A*63
    BOOST_CHECK_CLOSE( route[501].latitude() , ddmTodd("3722.5563") , percentageAccuracy );
    BOOST_CHECK_CLOSE( route[501].longitude() , -ddmTodd("00559.2403") , percentageAccuracy );
}

BOOST_AUTO_TEST_SUITE_END()

/////////////////////////////////////////////////////////////////////////////////////////
