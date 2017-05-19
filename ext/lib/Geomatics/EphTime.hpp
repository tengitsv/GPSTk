#ifndef GEOMATICS_EPHTIME_HPP
#define GEOMATICS_EPHTIME_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================
//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/// @file EphTime.hpp
/// gpstk::EphTime - encapsulates date and time-of-day, but only in formats applicable
/// to SolarSystemEphemeris, EarthOrientation and EOPStore, namely UTC, TT and TDB.
/// Conversion to and from DayTime or CommonTime should be automatic.

#include "TimeSystem.hpp"
#include "Exception.hpp"

// for non-gpstk, comment out the next line
#define EphTime_uses_gpstk_CommonTime 1
#ifdef EphTime_uses_gpstk_CommonTime
   #include "CommonTime.hpp"
   #define DayTime CommonTime
   #define DayTimeException CommonTimeException
   #include "MJD.hpp"
   #include "CivilTime.hpp"
   #include "TimeConverters.hpp"
#else
   #include "DayTime.hpp"
#endif

namespace gpstk
{
   /// @defgroup timegroup GPStk Time Group
   //@{

   /// Class implementing date+time, only in formats applicable solar system
   /// ephemeris and earth orientation, namely UTC, TT and TDB.
   /// Conversion to and from DayTime is implicit through casts defined here.
   class EphTime {
   private:
      long iMJD;           ///< integer MJD
      double dSOD;         ///< double seconds of day
      TimeSystem system;   ///< time system, limited to Unknown, UTC, TT, TDB

   public:
      /// empty constructor
      EphTime() : iMJD(0), dSOD(0.0), system(TimeSystem::Unknown) { }

      /// constructor, input int mjd, seconds of day, and system
      EphTime(int imjd, double dsod, TimeSystem sys=TimeSystem::UTC)
         : iMJD(imjd), dSOD(dsod), system(sys) {}

      /// constructor from full MJD and system
      EphTime(double mjd, TimeSystem sys=TimeSystem::UTC) : system(sys)
      {
         setMJD(mjd);
         setTimeSystem(sys);
      }

      /// convert systems
      /// @param ts TimeSystem to be converted to
      /// @throw if Correction does, if input system is Unknown
      void convertSystemTo(const TimeSystem& ts) throw(Exception)
      {
         try {
            long jday(static_cast<long>(iMJD + dSOD/SEC_PER_DAY + MJD_JDAY));
            int yy,mm,dd;
            convertJDtoCalendar(jday,yy,mm,dd);
            double dt, days = dd + dSOD/SEC_PER_DAY;
            // correct time
            dt = TimeSystem::Correction(system, ts, yy, mm, days);
            *this += dt;
            // set new system
            this->setTimeSystem(ts);
         }
         catch(Exception& e) { GPSTK_RETHROW(e); }
      }

      /// add seconds to this EphTime
      /// @param seconds double seconds to add
      EphTime& operator+=(double seconds)
      {
         dSOD += seconds;
         while(dSOD >= 86400.0) { dSOD -= 86400.0; iMJD++; }
         while(dSOD < 0.0) { dSOD += 86400.0; iMJD--; }
         return *this;
      }

      /// set the TimeSystem
      /// @param sys desired TimeSystem
      /// @throw if it is not allowed (one of UTC TT TBD)
      void setTimeSystem(TimeSystem sys) throw(Exception)
      {
         if(sys != TimeSystem::UTC &&
            sys != TimeSystem::TT &&
            sys != TimeSystem::TDB)
               GPSTK_THROW(Exception("Time system not allowed"));
         system = sys;
      }

      /// set to value of full MJD
      /// @param mjd long double MJD
      void setMJD(long double mjd) throw()
      {
         iMJD = long(mjd);
         dSOD = (mjd - static_cast<double>(iMJD)) * 86400.0;
      }

      /// Compute MJD
      /// @return long integer MJD
      long lMJD(void) const throw()
      {
         return iMJD;
      }

      /// Compute MJD
      /// @return full double MJD
      double dMJD(void) const throw()
      {
         return (static_cast<double>(iMJD) + dSOD/86400.);
      }

      /// @return seconds of day
      double secOfDay(void) const throw()
      {
         return dSOD;
      }

      /// @return year
      int year(void) const throw()
      {
         long jday(static_cast<long>(iMJD + dSOD/SEC_PER_DAY + MJD_JDAY));
         int yy,mm,dd;
         convertJDtoCalendar(jday,yy,mm,dd);
         return yy;
      }

      /// constructor from DayTime; convert to UTC if system is not UTC|TT|TDB, and
      /// change Unknown to UTC.
      /// @param dt DayTime input
      /// @throw if convertSystemTo does, if input system is Unknown
      EphTime(const DayTime& dt) throw(Exception)
      {
         try {
#ifndef EphTime_uses_gpstk_CommonTime
         DayTime ttag(dt);
         TimeSystem sys = ttag.getTimeSystem();
         // if its invalid, set it to UTC
         if(sys == TimeSystem::Unknown)
            ttag.setTimeSystem(TimeSystem::UTC);
         // if its not UTC TT or TDB, convert to UTC
         else if(sys != TimeSystem::UTC &&
                 sys != TimeSystem::TT &&
                 sys != TimeSystem::TDB)
            ttag.convertSystemTo(TimeSystem::UTC);

         ttag.getMJDsecOfDay(iMJD,dSOD);           
         system = ttag.getTimeSystem();
#else
         CommonTime ct(dt);
         TimeSystem sys = ct.getTimeSystem();
         if(sys == TimeSystem::Unknown || sys == TimeSystem::Any)
            ct.setTimeSystem(TimeSystem::UTC);
         // if its not UTC TT or TDB, convert it to UTC
         else if(sys != TimeSystem::UTC &&
                 sys != TimeSystem::TT &&
                 sys != TimeSystem::TDB)
         {
            CivilTime civt;
            civt.convertFromCommonTime(ct);
            double dt = TimeSystem::Correction(sys, TimeSystem::UTC,
                                    civt.year, civt.month, civt.day);
            ct += dt;
         }

         MJD ctmjd;
         ctmjd.convertFromCommonTime(ct);
         iMJD = static_cast<long>(ctmjd.mjd);
         dSOD = (ctmjd.mjd - static_cast<long double>(iMJD)) * SEC_PER_DAY;
         system = ctmjd.getTimeSystem();
#endif
         }
         catch(Exception& e) { GPSTK_RETHROW(e); }
      }

      /// const cast EphTime to DayTime
      operator DayTime() const throw()
      {
#ifndef EphTime_uses_gpstk_CommonTime
         DayTime ttag(static_cast<long double>(iMJD), system);
         ttag += dSOD;
         return ttag;
#else
         MJD ctmjd;
         ctmjd.mjd = static_cast<long double>(iMJD + dSOD/SEC_PER_DAY);
         CommonTime ct = ctmjd.convertToCommonTime();
         return ct;
#endif
      }

      // no this is not a duplicate of the previous function
      /// non-const cast EphTime to DayTime
      operator DayTime() throw()
      {
#ifndef EphTime_uses_gpstk_CommonTime
         DayTime ttag(static_cast<long double>(iMJD), system);
         ttag += dSOD;
         return ttag;
#else
         MJD ctmjd;
         ctmjd.mjd = static_cast<long double>(iMJD + dSOD/SEC_PER_DAY);
         CommonTime ct = ctmjd.convertToCommonTime();
         return ct;
#endif
      }

      /// return string of form Week sow.sss
      /// @param prec precision in fixed float output
      std::string asGPSString(const int prec=2) const
      {
         std::ostringstream oss;
         int wk((iMJD-GPS_EPOCH_MJD)/7);
         double sow((iMJD-wk*7)*SEC_PER_DAY + dSOD);
         if(sow >= FULLWEEK) { sow -= FULLWEEK; wk++; }
         oss << wk << " " << std::fixed << std::setprecision(prec) << sow;
         return oss.str();
      }

      /// return string of form MJD HH:MM:SS.ss
      /// @param prec precision in fixed float output
      std::string asMJDString(const int prec=3) const
      {
         std::ostringstream oss;
         int hh,mm;
         double sec;
         convertSODtoTime(dSOD,hh,mm,sec);
         oss << iMJD
            << " " << std::setfill('0') << std::setw(2) << hh
            << ":" << std::setw(2) << mm << ":" << std::setfill(' ')
            << std::fixed << std::setprecision(prec) << sec;
         return oss.str();
      }

   }; // end class EphTime

   //@}

}  // end namespace

#endif   // GEOMATICS_EPHTIME_HPP
