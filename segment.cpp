// Skeleton code for B657 A4 Part 2.
// D. Crandall
//
//
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <math.h>
#include <CImg.h>
#include <assert.h>
#include <algorithm>
#include <limits>
#include <set>

using namespace cimg_library;
using namespace std;

class Point
{
public:
  Point() {}
  Point(int _col, int _row) : row(_row), col(_col) {}
  int row, col;
  bool operator==(const Point& P) const {
        return (P.col == col && P.row == row);
    }
};

void print(const CImg<double> &image)
{
  for(int i=0; i<image.height(); i++)
  {
    for(int j=0; j<image.width(); j++)
    {
      cout << image(j,i) << " ";
    }
    cout << endl;
  }
}

CImg<double> naive_segment(const CImg<double> &img, const vector<Point> &fg, const vector<Point> &bg)
{
  // implement this in step 2...
  //  this placeholder just returns a random disparity map
  
  CImg<double> fg_temp_mean(3,1);
  for(int j=0; j<3; j++)
    fg_temp_mean(j,0)=0;
  for(int i=0; i<fg.size(); i++)
    for(int j=0; j<3; j++)
      fg_temp_mean(j,0)+=img( fg[i].col, fg[i].row, 0, j);
  for(int j=0; j<3; j++)
      fg_temp_mean(j,0)/=fg.size();

  CImg<double> fg_temp_var(3,3);
  for(int i=0; i<3; i++) 
    for(int j=0; j<3; j++)
    {
      fg_temp_var(j,i)=0;
      for(int k=0; k<fg.size() && i==j; k++)
        fg_temp_var(j,i)+=pow( (img( fg[k].col, fg[k].row, 0, j)-fg_temp_mean(j,0)), 2);
      fg_temp_var(j,i)/=fg.size();
    }
  
  CImg<double> fg_var=fg_temp_var.invert();
  double det_fg_var=fg_var.det();

  double mean=0;
  int ct=0;
  vector<int> y,x;
  multiset<double> alph;
  vector<double> values;

  bool chk_b, chk_f;
  Point P;
  CImg<double> result(img.width(),img.height());
  for(int i=0; i<img.height(); i++)
    for(int j=0; j<img.width(); j++)
    {  
      //result(j, i, 0, 0) = rand() % 2;
      P.row=i;
      P.col=j;
      vector<Point>::const_iterator pos=find( bg.begin(), bg.end(), P);
      chk_b=false;
      chk_f=false;
      if(pos!=bg.end())
        chk_b=true;
      else
      {
        pos=find( fg.begin(), fg.end(), P);
	if(pos!=fg.end())
	  chk_f=true;
      }
      if(chk_f)
        result(j,i,0,0)=0;
      else if(chk_b)
      	result(j,i,0,0)=1;
      else
      {
	result(j,i,0,0)=0;
	CImg<double> fg_mean(3,1);
	for(int k=0; k<3; k++)
          fg_mean(k,0)=(img(j,i,0,k)-fg_temp_mean(k,0));
	CImg<double> temp1=fg_mean;
	temp1.transpose();
        CImg<double> gauss=fg_mean*fg_var*temp1;
	double alpha=(exp(-0.5 * gauss(0,0)))/sqrt(2*M_PI*det_fg_var);
	alpha=-1*(log(alpha));
        mean+=alpha;
        ct++;
	y.push_back(j);
        x.push_back(i);
        alph.insert(alpha);
        values.push_back(alpha);
	//if(alpha > beta)
	//  result(j,i,0,0)=1;
      }
    }

  multiset<double>::const_iterator it = alph.begin();
  advance(it, ct/2);
  double median = *it;
  mean=mean/ct;
  cout << median << " " << mean << endl;
  double beta = (median + mean)/2;
  for(int i=0; i<values.size(); i++)
  {
    if(values[i] > beta)
    	result(y[i],x[i],0,0)=1;
  }

  bool fchk=true;
  int count;
  while(fchk)
  {
  fchk=false;
  for( int i=1; i<(img.height()-1); i++)
    for( int j=1; j<(img.width()-1); j++)
    {
      count=0;
      for(int k=i-1;k<=(i+1);k++)
        for(int l=j-1;l<=(j+1);l++)
	{
	  if(!(i==k && j==l))
	  {
	    if(result(l,k)!=result(j,i))
	      count++;
	  }
	}
      if(count>=6)
      {	
        result(j,i)=(!(result(j,i)));
        if(!fchk)
	  fchk=true;
      }
    }
  }

  return result;
}

CImg<double> mrf_segment(const CImg<double> &img, const vector<Point> &fg, const vector<Point> &bg)
{
  // implement this in step 3...
  //  this placeholder just returns a random disparity map by calling naive_segment
  return naive_segment(img, fg, bg);
}

// Take in an input image and a binary segmentation map. Use the segmentation map to split the 
//  input image into foreground and background portions, and then save each one as a separate image.
//
// You'll just need to modify this to additionally output a disparity map.
//
void output_segmentation(const CImg<double> &img, const CImg<double> &labels, const string &fname)
{
  // sanity checks. If one of these asserts fails, you've given this function invalid arguments!
  assert(img.height() == labels.height());
  assert(img.width() == labels.width());

  CImg<double> img_fg = img, img_bg = img;

  for(int i=0; i<labels.height(); i++)
    for(int j=0; j<labels.width(); j++)
      {
	if(labels(j,i) == 0)
	  img_fg(j,i,0,0) = img_fg(j,i,0,1) = img_fg(j,i,0,2) = 0;
	else if(labels(j,i) == 1)
	  img_bg(j,i,0,0) = img_bg(j,i,0,1) = img_bg(j,i,0,2) = 0;
	else
	  assert(0);
      }

  img_fg.get_normalize(0,255).save((fname + "_fg.png").c_str());
  img_bg.get_normalize(0,255).save((fname + "_bg.png").c_str());
}

int main(int argc, char *argv[])
{
  if(argc != 3)
    {
      cerr << "usage: " << argv[0] << " image_file seeds_file" << endl;
      return 1;
    }

  string input_filename1 = argv[1], input_filename2 = argv[2];

  // read in images and gt
  CImg<double> image_rgb(input_filename1.c_str());
  CImg<double> seeds_rgb(input_filename2.c_str());

  // figure out seed points 
  vector<Point> fg_pixels, bg_pixels;
  for(int i=0; i<seeds_rgb.height(); i++)
    for(int j=0; j<seeds_rgb.width(); j++)
      {
	// blue --> foreground
	if(max(seeds_rgb(j, i, 0, 0), seeds_rgb(j, i, 0, 1)) < 100 && seeds_rgb(j, i, 0, 2) > 100)
	  fg_pixels.push_back(Point(j, i));

	// red --> background
	if(max(seeds_rgb(j, i, 0, 2), seeds_rgb(j, i, 0, 1)) < 100 && seeds_rgb(j, i, 0, 0) > 100)
	  bg_pixels.push_back(Point(j, i));
      }

  // do naive segmentation
  CImg<double> labels = naive_segment(image_rgb, fg_pixels, bg_pixels);
  output_segmentation(image_rgb, labels, input_filename1 + "-naive_segment_result");

  // do mrf segmentation
  labels = naive_segment(image_rgb, fg_pixels, bg_pixels);
  output_segmentation(image_rgb, labels, input_filename1 + "-mrf_segment_result");

  return 0;
}
