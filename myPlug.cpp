// Copyright (c) 2012 The Foundry Visionmongers Ltd.  All Rights Reserved.

// Standard plug-in include files.
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/NukeWrapper.h"
 
using namespace DD::Image;

static const char* const CLASS = "myPlug"; // Name of the class (should be the same as the name of the final .dll file)
static const char* const HELP = "Example script - Horizontally shift RGB (individually)"; // The tooltip you see when hovering above the Help-button in Nuke

class myPlug : public Iop
{
	int shiftValue[3];

public:
	int maximum_inputs() const	{ return 1; }
	int minimum_inputs() const	{ return 1; }

	
  //! Constructor. Initialize user controls to their default values.
  myPlug (Node* node) : Iop (node)
  {
	shiftValue[0] = shiftValue[1] = shiftValue[2] = 0;
  }

  ~myPlug () {}
  
  void _validate(bool);
  void _request(int x, int y, int r, int t, ChannelMask channels, int count);
  
  //! This function does all the work.
  void engine ( int y, int x, int r, ChannelMask channels, Row& row );



  void knobs ( Knob_Callback );
  static const Iop::Description d;

  //! Return the name of the class.
  const char* Class() const { return d.name; }
  const char* node_help() const { return HELP; }

}; 


/*! This is a function that creates an instance of the operator, and is
   needed for the Iop::Description to work.
 */
static Iop* myPlugCreate(Node* node)
{
  return new myPlug(node);
}

/*! The Iop::Description is how NUKE knows what the name of the operator is,
   how to create one, and the menu item to show the user. The menu item may be
   0 if you do not want the operator to be visible.
 */
//const Iop::Description myPlug::d ( CLASS, "Examples/myPlug",
                                                     //myPlugCreate );


//const char* const myPlug::CLASS = "myPlug";
//const char* const myPlug::HELP = "Example Plugin";

void myPlug::_validate(bool for_real)
{
  copy_info(); // Output image with same properties as the input

}

void myPlug::_request(int x, int y, int r, int t, ChannelMask channels, int count)
{
	input0().request(x, y, r, t, channels, count); //Request image data
}


/*! For each line in the area passed to request(), this will be called. It must
   calculate the image data for a region at vertical position y, and between
   horizontal positions x and r, and write it to the passed row
   structure. Usually this works by asking the input for data, and modifying
   it.

 */
void myPlug::engine ( int y, int x, int r,
                              ChannelMask channels, Row& row )
{
	row.get(input0(), y, x, r, channels); //Fetch data

	foreach(z, channels)//For each channel (z) in channels (0 = R, 1 = G, 2 = B)
	{
		int rowlength = info().r(); //Screen width

		const int PixOffset = shiftValue[colourIndex(z)]; //colourIndex is defined in DD::Image, gets the current offset for each channel

		const float*INP = row[z]; //Copy the pointer
		float*OUTP = row.writable(z);//Allocate the output pointer

		for(int X = x; X < r; X++)//For each horizontal pixel in the row
		{
			int NewPixel = int(X + PixOffset);

			if(!(NewPixel > 0 && NewPixel < rowlength))//Check the pixel index for illegal values
			{
				NewPixel = -1; //When NewPixel doesn't exist within array INP, assign error value
			}

			float NewColor = (NewPixel == -1) ? 0: INP[NewPixel];
			OUTP[X] = NewColor;
		}

		
	}

}

void myPlug::knobs(Knob_Callback f)
{
	MultiInt_knob(f, shiftValue, 3, "value");
	Tooltip(f, "Set translation in RGB");
}

static Iop* build(Node *node) {
	return new NukeWrapper(new myPlug(node));
}
const Iop::Description myPlug::d("myPlug", "myPlug", build); // Nuke reads this to get the name of your plugin in the UI. Make sure this is the same as the name of the class and the final .dll file!




