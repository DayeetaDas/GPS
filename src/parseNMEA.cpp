#include "earth.h"
#include "parseNMEA.h"
#include<iostream>
#include<sstream>
#include<stdexcept>
#include<vector>
#include<ctype.h>
#include<string>
using namespace GPS;
using namespace Earth;

int const sentenceLength = 9; //declared constant values to avoid magic numbers
int const prefixLength = 3;
int const hexaLength = 2;

namespace NMEA
{

  bool isWellFormedSentence(std::string sentence)
  {
      std::string prefix ("$GP"); //declaration of prefix
      std::string checkprefix;

      if(sentence.length() < sentenceLength){ //Minimum sentence length is checked
          return false;
      }

      std::string sequence = sentence.substr(prefixLength,3); //predefined function takes the position and length of string
      int unsigned counter = 0;

            for(counter = 0;counter<prefixLength;counter++)
            {
                checkprefix = checkprefix + sentence.at(counter); //concatenation of characters to make prefix check
            }

            if (checkprefix != prefix){ //checks if prefix is equivilant
                return false;
            }
            else{
                for(counter = 0; counter<prefixLength;counter++){
                    if(!isalpha(sequence.at(counter))){ //check if characters are in the alphabet
                        return false;
                    }
                    continue;
                }
                if (sentence.at(sentence.length()-3) != '*') //check for * in correct position.
                {
                    return false;
                }
                for(counter = 6; counter < sentence.length()-3;counter++){
                    if (sentence.at(counter) == '*' || sentence.at(counter) == '$'){ //check if * or $ are in incorrect index
                        return false;
                    }
                    continue;
                    }
                for(counter = sentence.length() - hexaLength; counter< sentence.length(); counter++)
                    if (!isxdigit(sentence.at(counter))) //checks if characters are hexdecimal digit
                    {
                        return false;
                }
            }

      return true;
  }

  bool hasValidChecksum(std::string sentence)
  {
      std::string str= sentence; //sentence is declared as str
      char one = str.back(); //reference to the last character of the string
      str.pop_back();//remove the last character of the string
      char two= str.back();
      str= two;
      str+=one;
      int checksum= std::stoul(str, nullptr, 16); //converts string to unsigned long value.
      int check= sentence[1];
      int checkVal;
      for(int unsigned i=2; i< sentence.length(); i++)
      {
          if(sentence[i]== '*'){ // * cannot be in hex value
              break;
          }
          else{
              checkVal= sentence[i];
          }
          check^=checkVal; //XOR of characters
      }
      if(check== checksum){ //check to see if checksum is equivilant
          return true;
      }
      return false;
  }

  SentenceData extractSentenceData(std::string sentence)
    {
        std::pair<std::string, std::vector<std::string>> P;//define an object P for the pair
                  /* data stores the sentence data excluding the prefix, that is $GP
                   * newdata stores the sentence format and the positions excluding the checksum part of data*/
                   std::string data, newdata;
                   std::string prefix= "$GP";
                   std::size_t pos= sentence.find(prefix);//pos is used to store the position of the prefix
                   if(pos!=std::string::npos)
                   {
                       data= sentence.erase(pos, prefix.length());
                   }
                   P.first= data.substr(0, 3);
                   std::size_t remove= data.find(P.first);
                       if(remove!= std::string::npos)
                       {
                           data.erase(remove, P.first.length());
                       }
                       if(data.length()==3)
                       {
                           return {P.first,{}};
                       }
                       else if(data.length()!=3)
                       {
                           newdata= data.substr(1,data.length()-4)+",";
                           std::string dl= ",";std::string element;
                           size_t pos1=0;//pos1 stores the position of the ',' in the given string
                           while((pos1= newdata.find(dl))!=std::string::npos)
                           {
                               element= newdata.substr(0, pos1);
                               P.second.push_back(element);
                               newdata.erase(0, pos1+dl.length());
                           }
                       }
                       return P;//returns the pair object

    }

    GPS::Position positionFromSentenceData(SentenceData P)
    {
        std::string file_format= P.first;//file_format stores the "GLL", "GGA" or "RMC" value type
        std::vector<std::string> data= P.second;//data stores the positions excluding the format
        /* pos is used to find the position of the latitude, longitude and height
         * size stores the size of the data for a particular file_format*/
        int pos=0, size = 0;
        std::string lat=" ", lon=" ", h="0.0001";
        char dir1=' ', dir2=' ';//dir1 and dir2 stores the northing and easting values respectively
        if(data.empty())
            throw std::invalid_argument("Empty data field!!!");
        if(file_format=="GLL")
            size=5;
        else if(file_format=="RMC")
        {size=11; pos= 2;}
        else if(file_format=="GGA")
        {size=14; pos=1; h=data[8];}
        else
            throw std::invalid_argument("Invalid data size for"+file_format);
        lat= data[pos];
        lon= data[pos+2];
        dir1= data[pos+1][0];
        dir2= data[pos+3][0];
        try
        {
            return GPS::Position(lat, dir1, lon, dir2, h);
        }
        catch(int e)
        {
            throw std::invalid_argument("Invalid data types! Cannot compute position!");
        }
        return GPS::Earth::NorthPole;
    }

    Route routeFromLog(std::istream &route)
    {
        std::string sentence;
        SentenceData data;//data stores the position values for a particular NMEA sentence
        std::vector<Position> path;//path is the vector that stores all the valid positions from the NMEA sentences
            while(getline(route, sentence))
              {
                  if(isWellFormedSentence(sentence) && hasValidChecksum(sentence))
                  {
                      data = extractSentenceData(sentence);
                      try
                      {
                          path.push_back(positionFromSentenceData(data));
                      }
                      catch(...)
                      {

                      }
                  }
              }
              return path;
       }
  }










