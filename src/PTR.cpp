#include "PTR.h"

// **********************************************************
//  Given polygon represented by points, decompose it into
//  some rectangles, stored in vBoxes
// **********************************************************
bool
PTR::Polygon2Rectangle(oaPointArray & points, vector<oaBox> & vBoxes)
//{{{
{
#ifdef _DEBUG_PTR
  //cout << "DEBUG| PTR::Polygon2Rectangle()" << endl;
#endif

  compress(points);

  bool bb = findLine(points);
  if (bb == true) {
    cout << "WARNING| find line" << endl;
    exit(0);
  }

  int debuground = 0;
  while (points.getNumElements() > 0) {
    oaPoint Pk, Pl, Pm;
    
    #ifdef _DEBUG_PTR
      printf ("DEBUG| before FindPkPlPm(), ");
      print(points);
    #endif
    
    bool bb = FindPkPlPm(points, Pk, Pl, Pm);
    if ( !bb ) {
    #ifdef _DEBUG_BEI
      cout << "WARNING| can NOT find Pk Pl Pm.  points.getNumElements() = " << points.getNumElements() << endl;
      print(points);
    #endif

      return false; ///
      //cout << "EXIT|PTR::Polygon2Rectangle()" << endl; ///
      //exit(0); ///
    }

    oaBox box(Pk.x(), Pk.y(), Pl.x(), Pm.y());
    vBoxes.push_back(box);
    #ifdef _DEBUG_PTR
      printf ("Pk = (%d, %d), Pl = (%d, %d) Pm = (%d, %d)\n", Pk.x(), Pk.y(), Pl.x(), Pl.y(), Pm.x(), Pm.y());
      printf ("DEBUG| add oaBox (%d %d), (%d %d)\n", box.left(), box.bottom(), box.right(), box.top());
    #endif

    F(points, Pk.x(), Pk.y());
    F(points, Pl.x(), Pl.y());
    F(points, Pk.x(), Pm.y());
    F(points, Pl.x(), Pm.y());

    debuground ++;
    //if (debuground > 5) break;
  }
  return true;
}
//}}}


// *****************************************************
//  Given points, find Pk, Pl and Pm
//  Pk: the leftmost of the lowest points
//  Pl: the next leftmost of the lowest points
//  Pm: 1) Xk <= Xm < Xl
//      2) Ym is lowest but Ym > Yk (Yk == Yl)
// *****************************************************
bool
PTR::FindPkPlPm(const oaPointArray points, oaPoint& Pk, oaPoint& Pl, oaPoint & Pm)
/*{{{*/
{
  if (points.getNumElements() < 4) {
  #ifdef _DEBUG_BEI
    printf ("DEBUG| points.getNumElements() = %d\n", points.getNumElements());
  #endif
    return false;
  }

  int min_y = INT_MAX; 
  int min_x = INT_MAX; int next_x = INT_MAX;

  // first round, determine Pk, Pl
  for (int i=0; i<points.getNumElements(); i++) {
    int y = points[i].y();
    if (y < min_y) {
      min_y = y;
      min_x = next_x = INT_MAX;
    }
    if (y > min_y) continue;

    // here: y == min_y
    int x = points[i].x();
    if (x < min_x) {
      next_x = min_x;
      min_x = x;
    }
    else if (x > min_x && x < next_x) {
      next_x = x;
    }
  }
  Pk.set(min_x, min_y);
  Pl.set(next_x, min_y);

  // second round, determine Ym (next_y)
  int Ym = INT_MAX;
  for (int i=0; i<points.getNumElements(); i++) {
    int x = points[i].x();
    int y = points[i].y();
    if (y <= min_y) continue;
    if (x < Pk.x() || x >= Pl.x()) continue;
    if (Ym > y) Ym = y;
  }

  // third round, determine Xm
  int Xm = INT_MAX;
  for (int i=0; i<points.getNumElements(); i++) {
    int x = points[i].x();
    int y = points[i].y();
    if (y != Ym) continue;
    if (Xm > x) Xm = x;
  }

  #ifdef _DEBUG_PTR
    if (Ym >= INT_MAX || Xm >= INT_MAX) {
      cout << "DEBUG| PTR::FindPkPlPm(), output the points" << endl;
      for (int i=0; i<points.getNumElements(); i++) {
        int x = points[i].x();
        int y = points[i].y();
        cout << x << ", " << y << endl;
      } // for i
    }
  #endif
#if 0
  if (Ym >= INT_MAX) {
    cout << Ym << endl;
    for (int i=0; i<points.getNumElements(); i++) {
      int x = points[i].x();
      int y = points[i].y();
      cout << x<<","<<y<<" \t";
    }
    cout << endl;
  }
#endif
  if (Ym >= INT_MAX) return false;
  if (Xm >= INT_MAX) return false;

  Pm.set(Xm, Ym);
  return true;
}
/*}}}*/


// F function
void
PTR::F(oaPointArray & points, int X, int Y)
/*{{{*/
{
  oaPoint point(X, Y);
  int find = points.find(point);
  
  if (find < 0) { // can NOT find, insert point(X, Y)
    points.append( point );
    #ifdef _DEBUG_PTR
      printf ("append point (%d, %d) \n", point.x(), point.y());
      print(points);
    #endif
  }
  else { // can find, remove point (X, Y)
    points.remove( point );
    #ifdef _DEBUG_PTR
      printf ("remove point (%d, %d) \n", point.x(), point.y());
      print(points);
    #endif
  }

  //cout << "EXIT| PTR::F()" << endl;
  //exit(0);
}
/*}}}*/

// use kind of stupid method
void
PTR::compress(oaPointArray & points, int difference)
/*{{{*/
{
  int num1 = points.getNumElements();

  bool bmodify = true;
  while (true == bmodify) {
    bmodify = false;
    for (int i = 0; i<points.getNumElements(); i++) {
      int x1 = points[i].x();  int y1 = points[i].y();
      for (int j=i+1; j<points.getNumElements(); j++) {
        int x2 = points[j].x();  int y2 = points[j].y();
        if ( x1 != x2 && abs(x1 - x2) <= difference ) {
          points[i].set(min(x1, x2), y1);
          points[j].set(min(x1, x2), y2);
          bmodify = true;
        }
        if ( y1 != y2 && abs(y1 - y2) <= difference ) {
          points[i].set(x1, min(y1, y2));
          points[j].set(x2, min(y1, y2));
          bmodify = true;
        }
      } // for j
    } // for i
  } // while (bmodify)

  points.compress();
  int num2 = points.getNumElements();

#ifdef _DEBUG_PTR
  if (num1 > num2) {
    printf ("DEBUG| PTR::compress(), num1 = %d  num2 = %d\n", num1, num2);
  }
#endif
}
/*}}}*/

// may cause some errors
void
PTR::compress_old(oaPointArray & points, int difference)
/*{{{*/
{
  int num1 = points.getNumElements();

  for (int i = 0; i<points.getNumElements(); i++) {
    int x1 = points[i].x();
    int y1 = points[i].y();
    for (int j = i+1; j<points.getNumElements(); j++) {
      int x2 = points[j].x();
      int y2 = points[j].y();
      if ( x1 != x2 && abs(x1 - x2) <= difference ) {
        int midx = (x1+x2)/2;
        points[i].set(midx, y1);
        points[j].set(midx, y2);
        printf ("DEBUG| x1=%d, x2=%d, midx=%d\n", x1, x2, midx); ///
      }
      if ( y1 != y2 && abs(y1 - y2) <= difference ) {
        int midy = (y1+y2)/2;
        points[i].set(x1, midy);
        points[j].set(x2, midy);
        printf ("DEBUG| y1=%d, y2=%d, midy=%d\n", y1, y2, midy); ///
      }
    } // for (j)
  } // for (i)

  points.compress();
  int num2 = points.getNumElements();

#ifdef _DEBUG_PTR
  //if (num1 > num2) {
    printf ("DEBUG| PTR::compress(), num1 = %d  num2 = %d\n", num1, num2);
  //}
#endif
}
/*}}}*/



// ******************************************************
//                Debug function
// ******************************************************
bool
PTR::findLine(const oaPointArray points)
{
  int size = points.getNumElements();
  for (int i=0; i<size; i++) {
    int j = i+1; int k = i+2;
    if (j >= size) continue;
    if (k >= size) continue;
    if (points[i].x() == points[j].x() && points[i].x() == points[k].x()) {
      #ifdef _DEBUG_PTR
        printf ("DEBUG| same x: %d %d %d\n", points[i].x(), points[j].x(), points[k].x());
      #endif
      return true;
    }
    if (points[i].y() == points[j].y() && points[i].y() == points[k].y()) {
      #ifdef _DEBUG_PTR
        printf ("DEBUG| same y: %d %d %d\n", points[i].y(), points[j].y(), points[k].y());
      #endif
      return true;
    }
  } // for i
  return false;
}

void
PTR::print(const oaPointArray points, int min_x, int min_y)
{
  printf ("points: ");
  for (int i=0; i<points.getNumElements(); i++) {
    int x = points[i].x();
    int y = points[i].y();
    printf ("(%d, %d) ", x-min_x, y-min_y);
  }
  printf ("\n");
}

int
PTR::getMinX(const oaPointArray points)
{
  int min_x = INT_MAX;
  for (int i=0; i<points.getSize(); i++) {
    int x = points[i].x();
    if (min_x <= x) continue;
    min_x = x;
  }
  return min_x;
}

int
PTR::getMinY(const oaPointArray points)
{
  int min_y = INT_MAX;
  for (int i=0; i<points.getSize(); i++) {
    int y = points[i].y();
    if (min_y <= y) continue;
    min_y = y;
  }
  return min_y;
}

