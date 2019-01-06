#include "tags.h"

class DialClass {

  int screenWidth = 800;
  int screenHeight = 480;

 // Store memory for entered digits and symbols
  int dialEntries[15];
  // current number of digits in display
  int digits; 

  bool error;
  bool warning;
  
  bool keydepressed = true;
  char voltDecade[3] = "  ";
  char curDecade[3] = "  ";
  bool negative = false;
  bool dialog = false;
  double mv;
  int vol_cur_type;


  
public:
  void open(int type, void (*closedFn)(int type, bool cancel));
  void clear();
  bool isDialogOpen();
  int  type();
  void handleKeypadDialog();
  void checkKeypress();
  float getMv();
private:
  double toMv();
  void validate(double mv);
  void transButton(int x, int y, int sz, const char* label, int fontsize);
  void showError(const char* text);
  void showWarning(const char* text);
  void renderInput(bool valid);
  void renderKeypad();
};

extern DialClass DIAL;

