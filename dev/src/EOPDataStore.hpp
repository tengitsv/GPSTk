#pragma ident "$Id$"

/**
* @file EOPDataStore.hpp
* 
*/

#ifndef GPSTK_EOPDATASTORE_HPP
#define GPSTK_EOPDATASTORE_HPP


//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================


#include "EpochDataStore.hpp"
#include "Triple.hpp"

namespace gpstk
{
      /** @addtogroup Precise Orbit Determination */
      //@{

      /**
       * Class to store and manage Earth Orientation data.
       *
       */
   class EOPDataStore : public EpochDataStore
   {
   public:
     
      typedef struct EOPData
      {
         double xp;        /// arcseconds
         double yp;        /// arcseconds
         double UT1mUTC;   /// seconds
         double dPsi;      /// arcseconds
         double dEps;      /// arcseconds
         
         EOPData() : xp(0.0), yp(0.0), UT1mUTC(0.0),dPsi(0.0), dEps(0.0)
         {}

         EOPData(double x, double y, double ut1_utc, double dpsi = 0.0, double deps = 0.0) 
            : xp(x), yp(y), UT1mUTC(ut1_utc), dPsi(dpsi), dEps(deps)
         {}
      };

         /// Default constructor
      EOPDataStore() : EpochDataStore(2)
      {}

         /// Default deconstructor
      virtual ~EOPDataStore() {}
      
         /// Add to the store directly
      void addEOPData(const DayTime& utc,const EOPData& data)
         throw();

         /// Get the data at the given epoch and return it.
      EOPData getEOPData(const DayTime& utc) const
         throw(InvalidRequest);


         /// Add EOPs to the store via a flat IERS file. 
         /// get finals.data from http://maia.usno.navy.mil/
      void loadIERSFile(std::string iersFile)
         throw(FileMissingException);


      void loadIGSFile(std::string igsFile)
         throw(FileMissingException);

   protected:


   }; // End of class 'EOPDataStore'

   
   std::ostream& operator<<(std::ostream& s, const EOPDataStore::EOPData& d);

      // @}

}  // End of namespace 'gpstk'


#endif   // GPSTK_EOPDATASTORE_HPP







