
//operator overloaded friend keyword, not same class object parameter
#include<iostream>
 
class clfloat;

class clint{ 
      private: 
      int mint;
 
      public:
      clint( int p ){//3
            this->mint = p;
      }//3
   
      int fget(  ){//5   
            return this->mint;
      }//5

      //this point will byref 
      clint operator+( clint a){//4
            clint r( 0 );
            r.mint = this->mint + a.mint;
            
            return r;
      }//4
      
      
      friend clfloat operator+( clint a, clfloat b  );
};
 

class clfloat{ //11
      private: 
      float mfloat;
 
      public:
      clfloat( float p ){//10
            this->mfloat = p;
      };//10
      
      clfloat(   ){//14
            this->mfloat = 0;
      };//14
      
      float fget(  ){//12   
            return this->mfloat;
      };//12

      //this point will byref 
      clfloat operator+( clfloat a){//4
            clfloat r( 0 );
            r.mfloat = this->mfloat + a.mfloat;
            
            return r;
      };//4
      
      friend clfloat operator+( clint a, clfloat b  );
};//11

clfloat operator+( clint a, clfloat b  ){   //2
      clfloat r;
      r.mfloat = a.mint + b.mfloat;
      return r;
} //2

int main(void) {//2
 
      clint first(  10);
      clfloat second( 21.12 );
      clfloat r(  0);
      r = first + second;
      
      std::cout << "\n\nr float " << r.fget(  ) << "\n";
       
      std::cout << "\n\n\nexit...\n";
 
      return 0;
}//2






