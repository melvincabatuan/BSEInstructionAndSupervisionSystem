// BSE_IntegratedSystem.cpp : Defines the entry point for the console application.
//

/*
program_state = 0  ---> default
program_state = 1  ---> display camera feed
program_state = 2  ---> delineate
program_state = 3  ---> palpate
program_state = 4  ---> paused
program_state = 5  ---> finished
//*/


//GUI Variables
Mat GUI, button_play, button_pause, button_restart, button_stop, default_frame, temp_frame, default_infobox, default_msgbox;
Rect GUI_framebox, GUI_button1, GUI_button2, GUI_msgbox, GUI_infobox;
int program_state = 0;
bool delineated = false;
int manX, manY;


//Palpation variables
int pCount=0;
int s=0, r=0, c=0;
char message1[200] = "Sample Message 1";
char message2[200] = "Sample Message 2";

//Breast area division variables
Rect Block[2][5][6];
Rect Block2[2][4][12];
Rect Quad[2][4];
Rect Side[2];
bool blockStatus1[2][5][6] = {{{0,0,0,0,0,0},{0,0,0,0,0,0}},{{0,0,0,0,0,0},{0,0,0,0,0,0}}};
Rect leftQuad[4], rightQuad[4];
int upperRows, lowerRows, outerCols, innerCols;

//Infobox variables
Rect infoBlock[2][4][12];
Rect infoQuad[2][4];
Rect infoSide[2];
bool blockStatus2[2][4][12] = {{{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}},{{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}}};
bool quadStatus[2][4] = {{0,0,0,0},{0,0,0,0}};
bool sideStatus[2] = {0,0};
bool BSEStatus = 0;
int quadBlock[2][4][12][2];
int rowLimit[4], colLimit[4];

//////////////////////////////////////////////RESET PROGRAM/////////////////////////////////////////////////////
void resetProgram()
{
	s = 0;
	r = 0;
	c = 0;

	pCount = 0;

	for(int a=0; a<2; a++)
		for(int b=0; b<5; b++)
			for(int c=0; c<6; c++)
				blockStatus1[a][b][c] = 0;

	sprintf(message1,"");
	sprintf(message2,"");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////RESET GUI///////////////////////////////////////////////////////
void resetGUI()
{
	switch(program_state)
	{
		case 0: //default
			default_frame.copyTo(GUI(GUI_framebox));
			default_infobox.copyTo(GUI(GUI_infobox));
			default_msgbox.copyTo(GUI(GUI_msgbox));
			button_play.copyTo(GUI(GUI_button1));
			button_stop.copyTo(GUI(GUI_button2));
			break;
		case 1: //show camera capture
			default_frame.copyTo(GUI(GUI_framebox));
			default_infobox.copyTo(GUI(GUI_infobox));
			default_msgbox.copyTo(GUI(GUI_msgbox));
			button_play.copyTo(GUI(GUI_button1));
			button_stop.copyTo(GUI(GUI_button2));
			break;
		case 2: //delineate
			default_infobox.copyTo(GUI(GUI_infobox));
			default_msgbox.copyTo(GUI(GUI_msgbox));
			button_play.copyTo(GUI(GUI_button1));
			button_stop.copyTo(GUI(GUI_button2));
			break;
		case 3: //palpate
			default_frame.copyTo(GUI(GUI_framebox));
			default_infobox.copyTo(GUI(GUI_infobox));
			default_msgbox.copyTo(GUI(GUI_msgbox));
			button_pause.copyTo(GUI(GUI_button1));
			button_stop.copyTo(GUI(GUI_button2));
			break;
		case 4: //paused
			default_infobox.copyTo(GUI(GUI_infobox));
			default_msgbox.copyTo(GUI(GUI_msgbox));
			button_play.copyTo(GUI(GUI_button1));
			button_stop.copyTo(GUI(GUI_button2));
			break;
		case 5: //finished
			default_frame.copyTo(GUI(GUI_framebox));
			default_infobox.copyTo(GUI(GUI_infobox));
			default_msgbox.copyTo(GUI(GUI_msgbox));
			button_restart.copyTo(GUI(GUI_button1));
			button_stop.copyTo(GUI(GUI_button2));
			break;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////REGION HIGHLIGHTING///////////////////////////////////////////////////
void highlight()
{
	Rect tempBox;

	if(program_state > 2)
	{
		for(int side=0; side<2; side++)
		{
			if( manX>infoSide[side].x 
				&& manY>infoSide[side].y 
				&& manX<infoSide[side].x+infoSide[side].width 
				&& manY<infoSide[side].y+infoSide[side].height
				)
			{
				tempBox = Side[side];
				tempBox.x = tempBox.x + GUI_framebox.x;
				tempBox.y = tempBox.y + GUI_framebox.y;
				GUI(infoSide[side]) = GUI(infoSide[side]) + Scalar(-40,60,60);
				rectangle(GUI,tempBox,Scalar(0,255,255),4,8,0);
				GUI(tempBox) = GUI(tempBox) + Scalar(0,100,100);
			}

			for(int quad=0; quad<4; quad++)
			{
				if( manX>infoQuad[side][quad].x 
					&& manY>infoQuad[side][quad].y 
					&& manX<infoQuad[side][quad].x+infoQuad[side][quad].width 
					&& manY<infoQuad[side][quad].y+infoQuad[side][quad].height
					)
				{
					tempBox = Quad[side][quad];
					tempBox.x = tempBox.x + GUI_framebox.x;
					tempBox.y = tempBox.y + GUI_framebox.y;
					GUI(infoQuad[side][quad]) = GUI(infoQuad[side][quad]) + Scalar(60,-40,-40);
					rectangle(GUI,tempBox,Scalar(255,0,0),4,8,0);
					GUI(tempBox) = GUI(tempBox) + Scalar(100,0,0);
				}

				for(int row=0; row<rowLimit[quad]; row++)
				{
					for(int col=0; col<colLimit[quad]; col++)
					{
						if( manX>infoBlock[side][quad][col + colLimit[quad]*row].x 
							&& manY>infoBlock[side][quad][col + colLimit[quad]*row].y 
							&& manX<infoBlock[side][quad][col + colLimit[quad]*row].x+infoBlock[side][quad][col + colLimit[quad]*row].width 
							&& manY<infoBlock[side][quad][col + colLimit[quad]*row].y+infoBlock[side][quad][col + colLimit[quad]*row].height
							)
						{
							GUI(infoBlock[side][quad][col + colLimit[quad]*row]) = GUI(infoBlock[side][quad][col + colLimit[quad]*row]) + Scalar(-40,-40,60);
							tempBox = Block[side][quadBlock[side][quad][col + colLimit[quad]*row][0]][quadBlock[side][quad][col + colLimit[quad]*row][1]];
							tempBox.x = tempBox.x + GUI_framebox.x;
							tempBox.y = tempBox.y + GUI_framebox.y;
							rectangle(GUI,tempBox,Scalar(0,0,255),4,8,0);
							GUI(tempBox) = GUI(tempBox) + Scalar(0,0,100);
						}
					}
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////MOUSE CALLBACK FUNCTION////////////////////////////////////////////////
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	//Button click
	if( event == EVENT_LBUTTONDOWN )
	{

		if( x>GUI_button1.x && y>GUI_button1.y && x<GUI_button1.x+GUI_button1.width && y<GUI_button1.y+GUI_button1.height)
		{
			GUI(GUI_button1) = GUI(GUI_button1) + Scalar(75,75,75);
			
			switch(program_state)
			{
				case 0:
					program_state = 1;
					break;

				case 1:
					program_state = 2;
					break;

				case 2:
					if(delineated == false)
						delineated = true;
					else
						program_state = 3;

					break;

				case 3:
					program_state = 4;
					break;

				case 4:
					program_state = 3;
					break;

				case 5:
					program_state = 1;
					resetProgram();
					break;
			}

		} else if( x>GUI_button2.x && y>GUI_button2.y && x<GUI_button2.x+GUI_button2.width && y<GUI_button2.y+GUI_button2.height)
		{
			GUI(GUI_button2) = GUI(GUI_button2) + Scalar(75,75,75);
			
			program_state = 0;
			resetProgram();
		}
	}

	//Button release
	if( event == EVENT_LBUTTONUP )
		resetGUI();

	//Button hover
	if( event == EVENT_MOUSEMOVE )
	{
		manX = x;
		manY = y;

		resetGUI();
		if( x>GUI_button1.x && y>GUI_button1.y && x<GUI_button1.x+GUI_button1.width && y<GUI_button1.y+GUI_button1.height)
		{
			GUI(GUI_button1) = GUI(GUI_button1) + Scalar(-40,-40,60);
		} else if( x>GUI_button2.x && y>GUI_button2.y && x<GUI_button2.x+GUI_button2.width && y<GUI_button2.y+GUI_button2.height)
		{
			GUI(GUI_button2) = GUI(GUI_button2) + Scalar(-40,-40,60);
		}

		highlight();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// ////////////INITIALIZE GUI/////////////////////////////////////////////////////
void initGUI()
{
	GUI_framebox.width = 640;
	GUI_framebox.height = 480;
	GUI_framebox.x = 22;
	GUI_framebox.y = 22;

	GUI_button1.width = 150;	
	GUI_button1.height = 150;
	GUI_button1.x = 678;
	GUI_button1.y = 518;

	GUI_button2.width = 150;
	GUI_button2.height = 150;
	GUI_button2.x = 835;
	GUI_button2.y = 518;

	GUI_infobox.width = 307;
	GUI_infobox.height = 500;
	GUI_infobox.x = 678;
	GUI_infobox.y = 12;

	GUI_msgbox.width = 660;
	GUI_msgbox.height = 150;
	GUI_msgbox.x = 12;
	GUI_msgbox.y = 518;

	GUI = imread("Files/GUI.png",CV_LOAD_IMAGE_COLOR);
	default_frame = imread("Files/default_frame.png",CV_LOAD_IMAGE_COLOR);
	temp_frame = imread("Files/temp_frame.jpg",CV_LOAD_IMAGE_COLOR);
	Size dsize(640,480);
	resize(default_frame,default_frame,dsize);
	resize(temp_frame,temp_frame,dsize);
	button_play = imread("Files/button_play.png",CV_LOAD_IMAGE_COLOR);
	button_pause = imread("Files/button_pause.png",CV_LOAD_IMAGE_COLOR);
	button_restart = imread("Files/button_restart.png",CV_LOAD_IMAGE_COLOR);
	button_stop = imread("Files/button_stop.png",CV_LOAD_IMAGE_COLOR);
	default_infobox = imread("Files/default_infobox.png",CV_LOAD_IMAGE_COLOR);
	default_msgbox = imread("Files/default_msgbox.png",CV_LOAD_IMAGE_COLOR);

	resetGUI();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////UPDATE MESSAGE BOX///////////////////////////////////////////////////
void updateMsgBox()
{
	default_msgbox.copyTo(GUI(GUI_msgbox));

	Rect progressBar;
	progressBar.x = 10;
	progressBar.y = 10;
	progressBar.width = 640;
	progressBar.height = 20;
	Rect progressStatus = progressBar;
	if(pCount == 0)
		progressStatus.width = progressBar.width;
	else
		progressStatus.width = progressBar.width*pCount/3;

	if(program_state > 2)
	{
		rectangle(GUI(GUI_msgbox),progressBar,Scalar(0,255,0),1,8,0);
		rectangle(GUI(GUI_msgbox),progressStatus,Scalar(0,255,0),-1,8,0);
	}

	putText(GUI(GUI_msgbox), message1 ,Point(13,60)	,FONT_HERSHEY_DUPLEX,0.8,Scalar(48,50,117),1,8,false);
	putText(GUI(GUI_msgbox), message2 ,Point(13,100),FONT_HERSHEY_DUPLEX,0.8,Scalar(48,50,117),1,8,false);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////UPDATE INFO BOX////////////////////////////////////////////////////
void updateInfoBox()
{
	default_infobox.copyTo(GUI(GUI_infobox));

	if(program_state > 2)
	{
		int rowCount=0;

		BSEStatus = true;

		for(int side=0; side<2; side++)
		{
			infoSide[side].x = GUI_infobox.x + 10 + 153*side;
			infoSide[side].y = GUI_infobox.y + 10;
			infoSide[side].width = 140;
			infoSide[side].height = 18;

			sideStatus[side] = true;

			char sideLabel[20];
			if(side == 0)
				sprintf(sideLabel,"Left Breast");
			else
				sprintf(sideLabel,"Right Breast");

			putText(GUI(infoSide[side]), sideLabel ,Point(0,15)	,FONT_HERSHEY_DUPLEX,0.5,Scalar(48,50,117),1,8,false);

			rowCount = 0;
			for(int quad=0; quad<4; quad++)
			{
				infoQuad[side][quad].x = GUI_infobox.x + 10 + 153*side;
				infoQuad[side][quad].y = GUI_infobox.y + 15 + 18*(2+rowCount) + 18*(quad-1) + 6*quad;
				infoQuad[side][quad].width = 140;
				infoQuad[side][quad].height = 18;

				Rect quadStatusCheckBox = Rect(Point(107,2),Point(125,18));
				quadStatus[side][quad] = true;

				char quadLabel[20];
				sprintf(quadLabel," Quadrant %d", quad+1);
				putText(GUI(infoQuad[side][quad]), quadLabel ,Point(0,15)	,FONT_HERSHEY_DUPLEX,0.48,Scalar(48,50,117),1,8,false);

				for(int row=0; row<rowLimit[quad]; row++)
				{
					for(int col=0; col<colLimit[quad]; col++)
					{
						char label[20];
						sprintf(label,"  Block %d-%d", quad+1, col+1+colLimit[quad]*row);

						infoBlock[side][quad][col + colLimit[quad]*row].x = GUI_infobox.x + 10 + 153*side;
						infoBlock[side][quad][col + colLimit[quad]*row].y = GUI_infobox.y + 15 + 18*(2+rowCount) + 24*quad;
						infoBlock[side][quad][col + colLimit[quad]*row].width = 140;
						infoBlock[side][quad][col + colLimit[quad]*row].height = 18;

						blockStatus2[side][quad][col + colLimit[quad]*row] = blockStatus1[side][quadBlock[side][quad][col + colLimit[quad]*row][0]][quadBlock[side][quad][col + colLimit[quad]*row][1]];
						quadStatus[side][quad] = quadStatus[side][quad] && blockStatus2[side][quad][col + colLimit[quad]*row];

						Rect statusCheckBox = Rect(Point(109,4),Point(123,16));



						if(blockStatus2[side][quad][col + colLimit[quad]*row] == true)
							rectangle(GUI(infoBlock[side][quad][col + colLimit[quad]*row]),statusCheckBox,Scalar(0,255,0),-1,8,0);
						else
							rectangle(GUI(infoBlock[side][quad][col + colLimit[quad]*row]),statusCheckBox,Scalar(0,0,255),-1,8,0);
						
						putText(GUI(infoBlock[side][quad][col + colLimit[quad]*row]), label ,Point(0,15)	,FONT_HERSHEY_DUPLEX,0.45,Scalar(48,50,117),1,8,false);


						//Block[side][row][col]
						if(quad == 0 || quad == 1)
							quadBlock[side][quad][col + colLimit[quad]*row][0] = row;
						else
							quadBlock[side][quad][col + colLimit[quad]*row][0] = row + upperRows;

						if(quad == 0 || quad == 3)
							quadBlock[side][quad][col + colLimit[quad]*row][1] = col;
						else
							quadBlock[side][quad][col + colLimit[quad]*row][1] = col + innerCols;

						

						rowCount++;
					}
				}

				sideStatus[side] = sideStatus[side] && quadStatus[side][quad];
				BSEStatus = BSEStatus && sideStatus[side];

				if(quadStatus[side][quad] == true)
					rectangle(GUI(infoQuad[side][quad]),quadStatusCheckBox,Scalar(0,255,0),-1,8,0);
				else
					rectangle(GUI(infoQuad[side][quad]),quadStatusCheckBox,Scalar(0,0,255),-1,8,0);
			}
		}

		if(BSEStatus == true)
			program_state = 5;
	}

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////DISPLAY GUI///////////////////////////////////////////////////////
void displayGUI(Mat frame_output)
{
	frame_output.copyTo(GUI(GUI_framebox));
	updateMsgBox();
	updateInfoBox();
		
	highlight();
		
	imshow("Boob App",GUI);
	setMouseCallback("Boob App", CallBackFunc, NULL);
	waitKey(1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------------------------------IDENTIFY BREAST AREA AND LOCATE NIPPLE-----------
/////////////////////////////////////////////////SKIN FILTER////////////////////////////////////////////////////
void skinFilter(Mat frame, Mat &skinmask)
{
	Mat Y(480,640, CV_8UC1, Scalar(0));
	Mat Cb(480,640, CV_8UC1, Scalar(0));
	Mat Cr(480,640, CV_8UC1, Scalar(0));

	for(int rr=0; rr<480; rr=rr+1)
	{
		for(int cc=0; cc<640; cc=cc+1)
		{
			int b = frame.at<Vec3b>(rr,cc)[0];
			int g = frame.at<Vec3b>(rr,cc)[1];
			int r = frame.at<Vec3b>(rr,cc)[2];
			Y.at<uchar>(rr,cc) = 0.299*r + 0.587*g + 0.11*b;
			Cb.at<uchar>(rr,cc) = b - Y.at<uchar>(rr,cc);
			Cr.at<uchar>(rr,cc) = r - Y.at<uchar>(rr,cc);
		}
	}

	//Dual Threshold Segmentation of Red Chrominance----------------------------
	threshold(Cr,Cr,127,255,THRESH_TOZERO_INV);
	threshold(Cr,skinmask,4,255,THRESH_BINARY);
	//--------------------------------------------------------------------------

	Mat element2 = getStructuringElement(cv::MORPH_CROSS, cv::Size(2 * 4 + 1, 2 * 4 + 1), cv::Point(4, 4) );
	erode(skinmask,skinmask,element2);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////LOCATE TORSO////////////////////////////////////////////////////
void locateTorso(Mat &frame_gray, Mat skinmask, Rect bodyROI, Rect &torsoROI, bool &found)
{	
	Mat rowsum = Mat::zeros(480,1,CV_32F);
	Mat colsum = Mat::zeros(640,1,CV_32F);
	Mat temp;
	int sumthres=0;

	torsoROI = bodyROI;

	//Calculate summation of each row----------------------------------------
	for(int row=0; row<480; row++)
	{
		rowsum.at<float>(row) = 0;
		for(int col=0; col<640; col++)
			if(skinmask.at<uchar>(row,col) > 0)
				rowsum.at<float>(row)++;
		
		if(rowsum.at<float>(row) > sumthres)
				sumthres=rowsum.at<float>(row);
	}
	//Determine upper boundary--------------------------------------------------
	for(int row=100; row>=0; row--)
	{
		if(rowsum.at<float>(row) < sumthres*0.75)
		{
			torsoROI.height = torsoROI.height-(row-torsoROI.y);
			torsoROI.y = row;
			break;
		}
	}
	/*Determine lower boundary--------------------------------------------------
	for(int row=240; row<480; row++)
	{
		if(rowsum.at<float>(row) < sumthres*0.75)
		{
			torsoROI.height = torsoROI.height-(torsoROI.y+torsoROI.height-row);
			break;
		}
	}//*/

	//Calculate summation of each column----------------------------------------
	sumthres = 0;
	for(int col=0; col<640; col++)
	{
		colsum.at<float>(col) = 0;
		for(int row=torsoROI.y; row<torsoROI.height; row++)
			if(skinmask.at<uchar>(row,col) > 0)
				colsum.at<float>(col)++;
		
		if(colsum.at<float>(col) > sumthres)
				sumthres=colsum.at<float>(col);
	}
	//Determine left boundary--------------------------------------------------
	for(int col=0; col<320; col++)
	{
		if(colsum.at<float>(col) > sumthres*0.5)
		{
			torsoROI.width = torsoROI.width-(col-torsoROI.x);
			torsoROI.x = col;
			break;
		}
	}
	//Determine right boundary--------------------------------------------------
	for(int col=640; col>320; col--)
	{
		if(colsum.at<float>(col) > sumthres*0.5)
		{
			torsoROI.width = torsoROI.width-(torsoROI.x+torsoROI.width-col);
			break;
		}
	}

	//Draw vertically long contours-------------------------------------------
	bilateralFilter(frame_gray,temp,1,1,5);	
	Canny(temp,temp,40,80,3,false);


	IplImage* cvcann2=cvCloneImage(&(IplImage)temp);
	CvMemStorage *storage2 = cvCreateMemStorage(0);
	CvSeq *contours2 = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage2);
	cvFindContours(cvcann2, storage2, &contours2, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
	cvSet(cvcann2, cvScalar(0,0,0));

	for(; contours2 != NULL; contours2 = contours2->h_next)
	{
		Rect testROI = cvBoundingRect(contours2,0);
		if(testROI.height > testROI.width)
			cvDrawContours(cvcann2, contours2, Scalar(255), cvScalarAll(255), 0, 3);
	}
	temp = cvarrToMat(cvcann2,false,true,0);


	//Search window------------------------------------------
	Rect Swindow;
	Swindow.width = 40;
	Swindow.height = torsoROI.height;
	Swindow.y = torsoROI.y;

	Point r1,r2;
	r1.y = 0;
	r2.y = Swindow.height;
	float meanleft=0, meanright=0;

	int cols = torsoROI.width-Swindow.width;
	for(int cc=cols/2-2*Swindow.width; cc>=Swindow.width; cc--)
	{
		Swindow.x = torsoROI.x + cc;
		Scalar mean;
		meanStdDev(temp(Swindow), mean, Scalar());
		if(mean[0] > meanleft)
		{
			meanleft = mean[0];
			r1.x = Swindow.x;
		}
	}

	Swindow.x = r1.x;
	rectangle(temp,Swindow,Scalar(100),2,8,0);

	for(int cc=cols/2+Swindow.width; cc<cols-Swindow.width; cc++)
	{
		Swindow.x = torsoROI.x + cc;
		Scalar mean;
		meanStdDev(temp(Swindow), mean, Scalar());
		if(mean[0] > meanright)
		{
			meanright = mean[0];
			r2.x = Swindow.x + Swindow.width;
		}
	}

	Swindow.x = r2.x - Swindow.width;
	rectangle(temp,Swindow,Scalar(255),2,8,0);


	torsoROI.x = r1.x;
	torsoROI.width = r2.x - r1.x;



	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////CURVE-FITTING FITNESS FUNCTION///////////////////////////////////////////
void curveFitness(Mat intImage, Rect breast, double &fitness, float A, float B, float C)
{
	float y;
	double bsum=0, wsum=0, sum=0;

	for(int col=0; col<breast.width*25/30; col=col+breast.width/30)
	{
		y = (float)(A*(col-breast.width/2)*(col-breast.width/2)/(-100) + B*(col-breast.width/2)/10 + C*breast.height);
		Rect window1, window2;
		window1.x = col;
		window1.y = y;
		window1.width = breast.width/30;
		window1.height = 20;

		double tl= intImage.at<int>((window1.y),(window1.x)); 
		double tr= intImage.at<int>((window1.y),(window1.x+window1.width)); 
		double bl= intImage.at<int>((window1.y+window1.height),(window1.x)); 
		double br= intImage.at<int>((window1.y+window1.height),(window1.x+window1.width)); 
  
		sum = br-bl-tr+tl;
		wsum = wsum + sum;

		window2.x = col;
		window2.y = y+20;
		window2.width = breast.width/30;
		window2.height = 20;

		tl= intImage.at<int>((window2.y),(window2.x)); 
		tr= intImage.at<int>((window2.y),(window2.x+window2.width)); 
		bl= intImage.at<int>((window2.y+window2.height),(window2.x)); 
		br= intImage.at<int>((window2.y+window2.height),(window2.x+window2.width)); 
  
		sum = br-bl-tr+tl; 
		bsum = bsum + sum;
	}

	fitness = (wsum-bsum);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////CURVE-FITTING///////////////////////////////////////////////////
void fitCurve(Mat sample, Rect ROI, int &yMax, char side[20])
{
	Mat sample2(ROI.height+ROI.height/2,ROI.width,CV_8UC1,Scalar(0));
	Rect breast;
	breast.width = ROI.width;
	breast.height = ROI.height;

	sample.copyTo(sample2(Rect(Point(0,0),Point(ROI.width,ROI.height))));

	
	Mat intImage(ROI.width, ROI.height, CV_32F,Scalar(0));
	integral(sample2, intImage, -1);

	double sum,bsum=0,wsum=0,fitness=0;

	float A=0.65, B=-5, C=0.8, y;
	float AA, BB, CC, maxFit=-100000;

	for(C=0.5; C<=0.8; C=C+0.01)
	{
		for(B=-7; B<=7; B=B+0.2)
		{
			for(A=0.4; A<=2; A=A+0.02)
			{
				curveFitness(intImage,breast,fitness,A,B,C);
				
				if(fitness > maxFit)
				{
					maxFit = fitness;
					AA = A;
					BB = B;
					CC = C;
				}
			}
		}
	}

	//*
	for(int col=0; col<breast.width; col=col+breast.width/30)
	{
		y = (float)(AA*(col-breast.width/2)*(col-breast.width/2)/(-100) + BB*(col-breast.width/2)/10 + CC*breast.height);
		Rect window1, window2;
		window1.x = col;
		window1.y = y;
		window1.width = breast.width/30;
		window1.height = 20;

		window2.x = col;
		window2.y = y+20;
		window2.width = breast.width/30;
		window2.height = 20;

		rectangle(sample2,window1,Scalar(255),1,8,0);
		rectangle(sample2,window2,Scalar(255),1,8,0);
	}
	//*/

	yMax = 0;

	for(int col=0; col<breast.width; col=col++)
	{
		y = (float)(AA*(col-breast.width/2)*(col-breast.width/2)/(-100) + BB*(col-breast.width/2)/10 + CC*breast.height) + 20;
		sample2.at<uchar>(y,col) = 200;

		if(y>yMax)
			yMax = y;
	}	

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////LOCATE BREASTS///////////////////////////////////////////////////
void locateBreast(Rect ROI, Mat &frame2, Rect &BreastBox, char side[20], bool &found)
{
	BreastBox = ROI;
	if (side == "left")
	{
		ROI.x = ROI.x + BreastBox.width/4;
		ROI.width = ROI.width-BreastBox.width*0.25;
	} else if(side == "right")
	{
		ROI.x = ROI.x;
		ROI.width = ROI.width-BreastBox.width*0.25;
	}

	Mat halfTorso;
	frame2(ROI).copyTo(halfTorso);
	Mat halfTorso2;
	cvtColor(halfTorso,halfTorso2,CV_BGR2GRAY);
	bilateralFilter(halfTorso2,halfTorso,1,1,5);	
	//Canny(halfTorso,halfTorso,30,60,3,false);
	//Canny(halfTorso,halfTorso,500,600,5,false);
	
	Mat grad_x, grad_y, grad;
	Mat abs_grad_x, abs_grad_y;
	Sobel( halfTorso, grad_x, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT );
	Sobel( halfTorso, grad_y, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );
	convertScaleAbs( grad_y, abs_grad_y );
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, halfTorso );
	Canny(halfTorso,halfTorso,500,600,5,false);

	//Contour Detection and Filtering
	IplImage* cvcann2=cvCloneImage(&(IplImage)halfTorso);
	CvMemStorage *storage2 = cvCreateMemStorage(0);
	CvSeq *contours2 = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage2);
	int maxwidth = 0;
	
	cvFindContours(cvcann2, storage2, &contours2, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
	cvSet(cvcann2, cvScalar(0,0,0));
	maxwidth = 0;
	for(; contours2 != NULL; contours2 = contours2->h_next)
	{
		Rect testROI = cvBoundingRect(contours2,0);
		CvScalar color = CV_RGB( 255,255,255 );
		if(testROI.width > testROI.height && testROI.width*testROI.height > 200)
		{
			cvDrawContours(cvcann2, contours2, color, cvScalarAll(255), 0, 3);
			rectangle(halfTorso,testROI,Scalar(150),1,8,0);
			if(testROI.width > maxwidth)
			{
				maxwidth = testROI.y + testROI.height;
			}
		}
	}
	halfTorso = Mat(cvcann2);


	//Curve-fitting method
	int yMax;
	fitCurve(halfTorso,ROI,yMax,side);
	BreastBox.height = yMax - 10;

	if(BreastBox.width < 100 || BreastBox.height < 100)
		found = false;
	else
		found = true;

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////RE-LOCATE BREASTS/////////////////////////////////////////////////
void relocateBreast(Mat &frame2_gray, Rect &BodyROI, Rect &breast, char side[20])
{
	Rect breast2 = breast;
	Point p1,p2;
	if(side == "left")
	{
		p1.x = 0.6*breast.x + 0.4*BodyROI.x;
		p1.y = breast.y;
		p2.x = breast.x + breast.width;
		p2.y = breast.y + breast.height;
	} else if(side == "right")
	{
		p1.x = breast.x;
		p1.y = breast.y;
		p2.x = 0.6*(breast.x + breast.width) + 0.4*(BodyROI.x + BodyROI.width);
		p2.y = breast.y + breast.height;
	}
	breast2 = Rect(p1,p2);
	Mat cropped, filtered;
	frame2_gray(breast2).copyTo(cropped);
	bilateralFilter(cropped,filtered,1,1,5);
	Canny(filtered,filtered,30,50,3,false);

	IplImage* cvcann2=cvCloneImage(&(IplImage)filtered);
	CvMemStorage *storage2 = cvCreateMemStorage(0);
	CvSeq *contours2 = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage2);
	cvFindContours(cvcann2, storage2, &contours2, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
	cvSet(cvcann2, cvScalar(0,0,0));
	int maxheight = 0;
	for(; contours2 != NULL; contours2 = contours2->h_next)
	{
		Rect testROI = cvBoundingRect(contours2,0);
		if(testROI.height > 1.5*testROI.width)
		{
			cvDrawContours(cvcann2, contours2, Scalar(255), cvScalarAll(255), 0, 3);
			rectangle(filtered,testROI,Scalar(150),1,8,0);
			if(testROI.height > maxheight)
			{
				maxheight = testROI.height;
				if(side == "left")
					p1.x = breast.x + testROI.x;
				else if(side == "right")
					p2.x = breast.x + testROI.x + testROI.width;
			}
		}
	}

	if(side == "left")
		if(abs(p1.x - breast.x) < 0.2*breast.width)
			breast = Rect(p1,p2);
	else if(side == "right")
		if(abs(p2.x - (breast.x+breast.width)) < 0.2*breast.width)
			breast = Rect(p1,p2);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////FITNESS FUNCTION//////////////////////////////////////////////////
void fitness(Rect ROI2, Mat intImage, float xR, float yR, float wSizeR, double &score)
{
	Rect Swindow;
	Point pA,pB,pC,pD,pE,pF,pG,pH,pI,pJ,pK,pL;
	double sum1,sum2,sum3,sum4,sum5;

	int wSize = wSizeR*ROI2.width/6;
	Swindow.width = wSize*6;
	Swindow.height = wSize*6;
				
	Swindow.x = xR*ROI2.width;
	Swindow.y = yR*ROI2.height;

	pA = Point( Swindow.x+(int)wSize*(0), Swindow.y+(int)wSize*(0) );
	pB = Point( Swindow.x+(int)wSize*(6), Swindow.y+(int)wSize*(0) );
	pC = Point( Swindow.x+(int)wSize*(0), Swindow.y+(int)wSize*(6) );
	pD = Point( Swindow.x+(int)wSize*(6), Swindow.y+(int)wSize*(6) );
	pE = Point( Swindow.x+(int)wSize*(1), Swindow.y+(int)wSize*(1) );
	pF = Point( Swindow.x+(int)wSize*(5), Swindow.y+(int)wSize*(1) );
	pG = Point( Swindow.x+(int)wSize*(1), Swindow.y+(int)wSize*(5) );
	pH = Point( Swindow.x+(int)wSize*(5), Swindow.y+(int)wSize*(5) );
	

	double tl1= intImage.at<int>(pA); 
	double tr1= intImage.at<int>(pB); 
	double bl1= intImage.at<int>(pC); 
	double br1= intImage.at<int>(pD);
	sum1 = br1-bl1-tr1+tl1; 

	double tl2= intImage.at<int>(pE); 
	double tr2= intImage.at<int>(pF); 
	double bl2= intImage.at<int>(pG); 
	double br2= intImage.at<int>(pH);
	sum2 = br2-bl2-tr2+tl2; 

	//*
	double A = (0.5*sum1 - 2*sum2) / ((255*wSize*wSize-255*wSize*2/3*wSize*2/3)*6*6);
	double C = abs(0.3-wSizeR);
	double D = 10;
	score = 1*A - 1*C + D;
	//*/

	/*
	double A = (sum1 - sum2) / (255*wSize*6*wSize*6 - 255*wSize*4/6*wSize*4/6);
	double B = 1 - (sum2) / (255*wSize*4/6*wSize*4/6);
	double C = abs(0.3-wSizeR)/0.3;
	double D = 10;
	score = 1*A + 2*B - 3*C + D;
	*/

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////NIPPLE SEARCH///////////////////////////////////////////////////
void nippleSearch(Rect ROI, Rect Swindow, Mat intImage, float &wSizeRsol, float &xRsol, float &yRsol)
{
	RNG rng( 0xFFFFFFFF );

	float wSizeR, xR, yR;
	double maxscore = 0, score=0;
	float incX = 2/((float)ROI.width);
	float incY = 2/((float)ROI.height);
	int wSize;

	//*Linear Search method------------------------------------------------------
	for(wSizeR=0.3; wSizeR<0.8; wSizeR=wSizeR+0.01)
	{
		wSize = wSizeR*ROI.width/6;
		Swindow.width = wSize*6;
		Swindow.height = wSize*6;
		for(xR=0; xR<1-wSizeR; xR=xR+incX)
		{
			for(yR=0; yR<1-wSizeR; yR=yR+incY)
			{
				fitness(ROI, intImage, xR, yR, wSizeR, score);

				if(score > maxscore)
				{
					maxscore = score;
					xRsol = xR;
					yRsol = yR;
					wSizeRsol = wSizeR;
				}
			}
		}
	}
	//*/
	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////LOCATE NIPPLES///////////////////////////////////////////////////
void locateNipple(Rect ROI, Mat &frame2, Rect &Swindow, char side[20])
{
	Mat Breast, HSV, HSV_V;

	if(side == "left")
	{
		ROI.x = ROI.x + 0.1*ROI.width;
		ROI.width = ROI.width - 0.1*ROI.width;
	} else if(side == "right")
	{
		ROI.width = ROI.width - 0.1*ROI.width;
	}

	ROI.y = ROI.y + 40;
	ROI.height = ROI.height - 40;

	frame2(ROI).copyTo(Breast);
	cvtColor(Breast,HSV_V,CV_BGR2GRAY);
	//vector<Mat> channels(3); split(HSV, channels); HSV_V = channels[2];

	float xRsol, yRsol, wSizeRsol;

	//Draw corners
	double qualityLevel = 0.01;
	double minDistance = 5;
	int blockSize = 3;
	int maxCorners = 20;

	vector<Point2f> corners;
	corners.reserve(maxCorners);
	goodFeaturesToTrack(HSV_V,corners,maxCorners,qualityLevel,minDistance,cv::Mat(),blockSize,false);

	//for (int i=0; i<corners.size(); i++)
		//HSV_V.at<uchar>(corners[i].y,corners[i].x) = HSV_V.at<uchar>(corners[i].y,corners[i].x) - 50;


	//calculate integral image
	Mat intImage(ROI.width, ROI.height, CV_32F,Scalar(0)); integral(HSV_V, intImage, -1);
	
	//optimization algorithm
	nippleSearch(ROI,Swindow,intImage,wSizeRsol,xRsol,yRsol);
	
	//translate optimization output to bounding box
	Swindow.width = wSizeRsol*ROI.width;
	Swindow.height = wSizeRsol*ROI.width;
	Swindow.x = xRsol*ROI.width + ROI.x;
	Swindow.y = yRsol*ROI.height + ROI.y;

	if(side == "left")
	{
		Swindow.x = Swindow.x - (1/9)*ROI.width;
	}

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////DIVIDE BREAST AREA////////////////////////////////////////////////
void divideBreastArea(Mat frame, Mat result, Rect leftBreast, Rect leftNipple, Rect rightBreast, Rect rightNipple)
{
	Point leftNippleCentroid = Point(leftNipple.x+leftNipple.width/2,leftNipple.y+leftNipple.height/2);

	//Rect leftQuad[4];

	leftQuad[0].x		= leftNippleCentroid.x;
	leftQuad[0].y		= leftBreast.y;
	leftQuad[0].width	= leftBreast.x + leftBreast.width - leftNippleCentroid.x;
	leftQuad[0].height	= leftNippleCentroid.y - leftBreast.y;

	leftQuad[1].x		= leftBreast.x;
	leftQuad[1].y		= leftBreast.y;
	leftQuad[1].width	= leftNippleCentroid.x - leftBreast.x;
	leftQuad[1].height	= leftNippleCentroid.y - leftBreast.y;

	leftQuad[2].x		= leftBreast.x;
	leftQuad[2].y		= leftNippleCentroid.y;
	leftQuad[2].width	= leftNippleCentroid.x - leftBreast.x;
	leftQuad[2].height	= leftBreast.y + leftBreast.height - leftNippleCentroid.y;

	leftQuad[3].x		= leftNippleCentroid.x;
	leftQuad[3].y		= leftNippleCentroid.y;
	leftQuad[3].width	= leftBreast.x + leftBreast.width - leftNippleCentroid.x;
	leftQuad[3].height	= leftBreast.y + leftBreast.height - leftNippleCentroid.y;

	float outerWidthRatio, innerWidthRatio, upperHeightRatio, lowerHeightRatio;

	outerWidthRatio  = (float)leftQuad[1].width/leftBreast.width;
	innerWidthRatio  = (float)leftQuad[0].width/leftBreast.width;
	upperHeightRatio = (float)leftQuad[1].height/leftBreast.height; 
	lowerHeightRatio = (float)leftQuad[2].height/leftBreast.height;

	if(outerWidthRatio > 0.45)
		outerCols = 2;
	else
		outerCols = 1;

	if(innerWidthRatio > 0.75)
		innerCols = 3;
	else
		innerCols = 2;

	if(upperHeightRatio > 0.75)
		upperRows = 4;
	else
		upperRows = 3;

	if(lowerHeightRatio > 0.29)
		lowerRows = 2;
	else
		lowerRows = 1;

	
	Rect leftBlock[4][4][3];

	for(int cols=0; cols<innerCols; cols++)
	{
		for(int rows=0; rows<upperRows; rows++)
		{
			leftBlock[0][rows][cols].x = leftQuad[0].x + leftQuad[0].width/innerCols*(innerCols-cols-1);
			leftBlock[0][rows][cols].y = leftQuad[0].y + leftQuad[0].height/upperRows*rows;
			leftBlock[0][rows][cols].width = leftQuad[0].width/innerCols;
			leftBlock[0][rows][cols].height = leftQuad[0].height/upperRows;

			Block[0][rows][cols] = leftBlock[0][rows][cols];
			Block2[0][0][rows + innerCols*cols] = leftBlock[0][rows][cols];

			rectangle(result,leftBlock[0][rows][cols],Scalar(50,0,0),1,8,0);
		}
	}

	for(int cols=0; cols<outerCols; cols++)
	{
		for(int rows=0; rows<upperRows; rows++)
		{
			leftBlock[1][rows][cols].x = leftQuad[1].x + leftQuad[1].width/outerCols*(outerCols-cols-1);
			leftBlock[1][rows][cols].y = leftQuad[1].y + leftQuad[1].height/upperRows*rows;
			leftBlock[1][rows][cols].width = leftQuad[1].width/outerCols;
			leftBlock[1][rows][cols].height = leftQuad[1].height/upperRows;

			Block[0][rows][cols+innerCols] = leftBlock[1][rows][cols];
			Block2[0][1][rows + outerCols*cols] = leftBlock[1][rows][cols];

			rectangle(result,leftBlock[1][rows][cols],Scalar(0,50,0),1,8,0);
		}
	}

	for(int cols=0; cols<outerCols; cols++)
	{
		for(int rows=0; rows<lowerRows; rows++)
		{
			leftBlock[2][rows][cols].x = leftQuad[2].x + leftQuad[2].width/outerCols*(outerCols-cols-1);
			leftBlock[2][rows][cols].y = leftQuad[2].y + leftQuad[2].height/lowerRows*rows;
			leftBlock[2][rows][cols].width = leftQuad[2].width/outerCols;
			leftBlock[2][rows][cols].height = leftQuad[2].height/lowerRows;

			Block[0][rows+upperRows][cols+innerCols] = leftBlock[2][rows][cols];
			Block2[0][2][rows + outerCols*cols] = leftBlock[2][rows][cols];

			rectangle(result,leftBlock[2][rows][cols],Scalar(0,0,50),1,8,0);
		}
	}

	for(int cols=0; cols<innerCols; cols++)
	{
		for(int rows=0; rows<lowerRows; rows++)
		{
			leftBlock[3][rows][cols].x = leftQuad[3].x + leftQuad[3].width/innerCols*(innerCols-cols-1);
			leftBlock[3][rows][cols].y = leftQuad[3].y + leftQuad[3].height/lowerRows*rows;
			leftBlock[3][rows][cols].width = leftQuad[3].width/innerCols;
			leftBlock[3][rows][cols].height = leftQuad[3].height/lowerRows;

			Block[0][rows+upperRows][cols] = leftBlock[3][rows][cols];
			Block2[0][3][rows + innerCols*cols] = leftBlock[3][rows][cols];

			rectangle(result,leftBlock[3][rows][cols],Scalar(0,50,50),1,8,0);
		}
	}

	Point rightNippleCentroid = Point(rightNipple.x+rightNipple.width/2,rightNipple.y+rightNipple.height/2);
	//Rect rightQuad[4];

	rightQuad[0].x		= rightBreast.x;
	rightQuad[0].y		= rightBreast.y;
	rightQuad[0].width	= rightNippleCentroid.x - rightBreast.x;
	rightQuad[0].height	= rightNippleCentroid.y - rightBreast.y;

	rightQuad[1].x		= rightNippleCentroid.x;
	rightQuad[1].y		= rightBreast.y;
	rightQuad[1].width	= rightBreast.x + rightBreast.width - rightNippleCentroid.x;
	rightQuad[1].height	= rightNippleCentroid.y - rightBreast.y;

	rightQuad[2].x		= rightNippleCentroid.x;
	rightQuad[2].y		= rightNippleCentroid.y;
	rightQuad[2].width	= rightBreast.x + rightBreast.width - rightNippleCentroid.x;
	rightQuad[2].height	= rightBreast.y + rightBreast.height - rightNippleCentroid.y;

	rightQuad[3].x		= rightBreast.x;
	rightQuad[3].y		= rightNippleCentroid.y;
	rightQuad[3].width	= rightNippleCentroid.x - rightBreast.x;
	rightQuad[3].height	= rightBreast.y + rightBreast.height - rightNippleCentroid.y;	

	Rect rightBlock[4][4][3];

	for(int cols=0; cols<innerCols; cols++)
	{
		for(int rows=0; rows<upperRows; rows++)
		{
			rightBlock[0][rows][cols].x = rightQuad[0].x + rightQuad[0].width/innerCols*cols;
			rightBlock[0][rows][cols].y = rightQuad[0].y + rightQuad[0].height/upperRows*rows;
			rightBlock[0][rows][cols].width = rightQuad[0].width/innerCols;
			rightBlock[0][rows][cols].height = rightQuad[0].height/upperRows;

			Block[1][rows][cols] = rightBlock[0][rows][cols];
			Block2[1][0][rows + innerCols*cols] = rightBlock[0][rows][cols];

			rectangle(result,Block2[1][0][rows + innerCols*cols],Scalar(50,0,0),1,8,0);
		}
	}

	for(int cols=0; cols<outerCols; cols++)
	{
		for(int rows=0; rows<upperRows; rows++)
		{
			rightBlock[1][rows][cols].x = rightQuad[1].x + rightQuad[1].width/outerCols*cols;
			rightBlock[1][rows][cols].y = rightQuad[1].y + rightQuad[1].height/upperRows*rows;
			rightBlock[1][rows][cols].width = rightQuad[1].width/outerCols;
			rightBlock[1][rows][cols].height = rightQuad[1].height/upperRows;

			Block[1][rows][cols+innerCols] = rightBlock[1][rows][cols];
			Block2[1][1][rows + outerCols*cols] = rightBlock[1][rows][cols];

			rectangle(result,Block2[1][1][rows + outerCols*cols],Scalar(0,50,0),1,8,0);
		}
	}

	for(int cols=0; cols<outerCols; cols++)
	{
		for(int rows=0; rows<lowerRows; rows++)
		{
			rightBlock[2][rows][cols].x = rightQuad[2].x + rightQuad[2].width/outerCols*cols;
			rightBlock[2][rows][cols].y = rightQuad[2].y + rightQuad[2].height/lowerRows*rows;
			rightBlock[2][rows][cols].width = rightQuad[2].width/outerCols;
			rightBlock[2][rows][cols].height = rightQuad[2].height/lowerRows;

			Block[1][rows+upperRows][cols+innerCols] = rightBlock[2][rows][cols];
			Block2[1][2][rows + outerCols*cols] = rightBlock[2][rows][cols];

			rectangle(result,Block2[1][2][rows + outerCols*cols],Scalar(0,0,50),1,8,0);
		}
	}

	for(int cols=0; cols<innerCols; cols++)
	{
		for(int rows=0; rows<lowerRows; rows++)
		{
			rightBlock[3][rows][cols].x = rightQuad[3].x + rightQuad[3].width/innerCols*cols;
			rightBlock[3][rows][cols].y = rightQuad[3].y + rightQuad[3].height/lowerRows*rows;
			rightBlock[3][rows][cols].width = rightQuad[3].width/innerCols;
			rightBlock[3][rows][cols].height = rightQuad[3].height/lowerRows;

			Block[1][rows+upperRows][cols] = rightBlock[3][rows][cols];
			Block2[1][3][rows + innerCols*cols] = rightBlock[3][rows][cols];

			rectangle(result,Block2[1][3][rows + innerCols*cols],Scalar(0,50,50),1,8,0);
		}
	}

	for(int count=0; count<4; count++)
	{
		Quad[0][count] = leftQuad[count];
		Quad[1][count] = rightQuad[count];
	}

	Side[0] = leftBreast;
	Side[1] = rightBreast;

	rowLimit[0] = upperRows;
	colLimit[0] = innerCols;

	rowLimit[1] = upperRows;
	colLimit[1] = outerCols;

	rowLimit[2] = lowerRows;
	colLimit[2] = outerCols;

	rowLimit[3] = lowerRows;
	colLimit[3] = innerCols;

	//imshow("haha",result); waitKey(10); waitKey(0);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////DELINEATE/////////////////////////////////////////////////////
void delineate(Mat frame, Mat &frame_output)
{
	Mat frame_gray, skinmask;
	Rect bodyROI, torsoROI, leftTorso, rightTorso, leftBreast, rightBreast, leftNipple, rightNipple;

	bool bodyFound, torsoFound, leftBreastFound, rightBreastFound;

	cvtColor(frame,frame_gray,CV_BGR2GRAY);
	skinFilter(frame, skinmask);

	imshow("Skin Mask",skinmask);

	bodyROI = Rect(Point(0,0),Point(640,480));
	locateTorso(frame_gray, skinmask, bodyROI, torsoROI, torsoFound);
	
	leftTorso = Rect(Point(torsoROI.x,torsoROI.y),Point(torsoROI.x+torsoROI.width/2,torsoROI.y+torsoROI.height));
	rightTorso = Rect(Point(torsoROI.x+torsoROI.width/2,torsoROI.y),Point(torsoROI.x+torsoROI.width,torsoROI.y+torsoROI.height));

	if(torsoFound == true)
	{
		locateBreast(leftTorso, frame, leftBreast, "left", leftBreastFound);
		locateBreast(rightTorso, frame, rightBreast, "right", rightBreastFound);

		Rect leftBreast2 = leftBreast;
		Rect rightBreast2 = rightBreast;

		relocateBreast(frame_gray, torsoROI, leftBreast2, "left");
		relocateBreast(frame_gray, torsoROI, rightBreast2, "right");

		leftBreast2.width = (rightBreast2.x+rightBreast2.width - leftBreast2.x)/2;
		rightBreast2.x = leftBreast2.x + leftBreast2.width;
		rightBreast2.width = leftBreast2.width;

		if(leftBreastFound==true && rightBreastFound==true)
		{
			locateNipple(leftBreast2,frame,leftNipple,"left");
			locateNipple(rightBreast2,frame,rightNipple,"right");
		}

	} else
	{
		//cout << "Torso not found" << endl;
	}

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int params[9] = {72, 87, 486, 305, 40, 80, 40, 80, 50};
////////////////////////////////////////////////////ANNOTATE////////////////////////////////////////////////////
void annotate(Mat frame, Rect &leftBreast, Rect &rightBreast, Rect &leftNipple, Rect &rightNipple, bool &delineated, Mat &result)
{
	namedWindow("Breast Parameters",CV_WINDOW_AUTOSIZE);
	namedWindow("Nipple Parameters",CV_WINDOW_NORMAL);

	if(waitKey(10) != ' ')
	{
		delineated = false;
	} else
	{
		delineated = true;
	}

	//Mat result;
	//frame.copyTo(result);

	createTrackbar("X",				"Breast Parameters",	&params[0], 320, NULL);
	createTrackbar("Y",				"Breast Parameters",	&params[1], 240, NULL);
	createTrackbar("Width",			"Breast Parameters",	&params[2], 640, NULL);
	createTrackbar("Height",		"Breast Parameters",	&params[3], 480, NULL);
		
	createTrackbar("Left X",		"Nipple Parameters",	&params[4], params[2]/2-params[8], NULL);
	createTrackbar("Left Y",		"Nipple Parameters",	&params[5], params[3]-params[8], NULL);
	createTrackbar("Right X",		"Nipple Parameters",	&params[6], params[2]/2-params[8], NULL);
	createTrackbar("Right Y",		"Nipple Parameters",	&params[7], params[3]-params[8], NULL);

	leftBreast.x		= params[0];
	leftBreast.y		= params[1];
	leftBreast.width	= params[2]/2;
	leftBreast.height	= params[3];

	rightBreast.x		= params[0] + params[2]/2;
	rightBreast.y		= params[1];
	rightBreast.width	= params[2]/2;
	rightBreast.height	= params[3];

	leftNipple.x		= params[0] + params[4];
	leftNipple.y		= params[1] + params[3] - params[5];
	leftNipple.width	= params[8];
	leftNipple.height	= params[8];

	rightNipple.x		= params[0] + params[2] - params[6] - params[8];
	rightNipple.y		= params[1] + params[3] - params[7];
	rightNipple.width	= params[8];
	rightNipple.height	= params[8];

	rectangle(result,leftBreast,Scalar(100,0,0),2,8,0);
	rectangle(result,rightBreast,Scalar(0,0,100),2,8,0);

	//rectangle(result,leftNipple,Scalar(100,0,0),2,8,0);
	//rectangle(result,rightNipple,Scalar(0,0,100),2,8,0);

	line(result,Point(leftNipple.x+leftNipple.width/2,leftBreast.y),Point(leftNipple.x+leftNipple.width/2,leftBreast.y+leftBreast.height),Scalar(255,0,0),2,8,0);
	line(result,Point(leftBreast.x,leftNipple.y+leftNipple.height/2),Point(leftBreast.x+leftBreast.width,leftNipple.y+leftNipple.height/2),Scalar(255,0,0),2,8,0);

	line(result,Point(rightNipple.x+rightNipple.width/2,rightBreast.y),Point(rightNipple.x+rightNipple.width/2,rightBreast.y+rightBreast.height),Scalar(0,0,255),2,8,0);
	line(result,Point(rightBreast.x,rightNipple.y+rightNipple.height/2),Point(rightBreast.x+rightBreast.width,rightNipple.y+rightNipple.height/2),Scalar(0,0,255),2,8,0);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------


static const double pi = 3.14159265358979323846;

//Optical Flow parameters
double qualityLevel = 0.01;
double minDistance = 5;
int blockSize = 3;
bool useHarrisDetector = false;
double k = 0.04;
int maxTrackbar = 100;

//Program Parameters
bool init = true;
int current_frame = 1;
int maxCorners = 100;
int mintrackcorners = maxCorners/20;
int maxDev = 30;

//Speed measurement variables
int tstart, tend;

//Manual annotation variables
bool clicked = false;

//////////////////////////////////////////AVERAGE-STD DEVIATION/////////////////////////////////////////////////
void AveStdDev(vector<Point2f> inputvector, double &xmean, double &ymean, double &xstd, double &ystd)
{
	Mat mean_;
	reduce(inputvector, mean_, CV_REDUCE_AVG, 1);
	xmean = mean_.at<float>(0,0);
	ymean = mean_.at<float>(0,1);

	std::vector<cv::Point2f> diff;
	diff = inputvector;
	for (int i=0; i<inputvector.size(); i++)
	{
		diff[i].x = pow((inputvector[i].x - xmean),2);
		diff[i].y = pow((inputvector[i].y - ymean),2);
	}

	Mat mean_2;
	reduce(diff, mean_2, CV_REDUCE_AVG, 1);
	xstd = mean_2.at<float>(0,0);
	ystd = mean_2.at<float>(0,1);
	xstd = sqrt(xstd);
	ystd = sqrt(ystd);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////HAND DETECTION/TRACKING FUNCTION///////////////////////////////////////////
void HandTracker2(Mat frame1, Mat frame2, Rect ROI, Point &prevloc, Point &handloc, vector<Point2f> &features, bool &tracked, Mat &Results)
{
	double xmean, ymean, xstd, ystd;	//Mean and Std.Dev. variables
    Mat prev, next;						//Frame(t-1) and Frame(t) containers
	Mat nextMask(480,640,CV_8UC1,Scalar(0));
	Mat prevMask(480,640,CV_8UC1,Scalar(0));
	vector<Point2f> corners, corners_b;	//Containers for corners detected(CornerDetection) and tracked(OpticalFlow)
	vector<uchar> features_found;		//Container for OpticalFlow result labels
	vector<float> feature_errors;		//Container for OpticalFlow result errors
	int sRadius = ROI.width*0.25;

	rectangle(nextMask,ROI,Scalar(255),-1,8,0);
	rectangle(prevMask,ROI,Scalar(255),-1,8,0);
	
	Rect initROI = ROI;
	initROI.height = initROI.height/4;
	//initROI.width  = initROI.width/2;

	if (init == true)
	{
		rectangle(Results,Point(initROI.x+initROI.width/2,initROI.y),Point(initROI.x+initROI.width,initROI.y+initROI.height),Scalar(0,255,255),10,8,0);

		corners.reserve(maxCorners);
		cvtColor(frame2(initROI), next, CV_BGR2GRAY);
		
		goodFeaturesToTrack(next,corners,maxCorners,qualityLevel*2,minDistance,cv::Mat(),blockSize,useHarrisDetector);

		for (int i=0; i<corners.size(); i++)
		{
			if (corners[i].x == ROI.x || corners[i].x == ROI.x+ROI.width || corners[i].y == ROI.y || corners[i].y == ROI.y+ROI.height)
			{
				corners.erase(corners.begin()+i);	i--;
			}
		}


		AveStdDev(corners,xmean,ymean,xstd,ystd);

		//*Draw corners
		for (int i=0; i<corners.size(); i++)
			//circle(Results(initROI), Point(corners[i].x,corners[i].y), 1, Scalar(0,0,255), 1, CV_AA);//*/


		//Eliminate outlying corners----------------------------------------------------------------------------
		for (int i=0; i<corners.size(); i++)
		{
			if (abs(corners[i].x-xmean)>xstd || abs(corners[i].y-ymean)>ystd)
			{
				corners.erase(corners.begin()+i);	i--;
			}
		}

		//*Draw remaining corners
		for (int i=0; i<corners.size(); i++)
			//circle(Results(initROI), Point(corners[i].x,corners[i].y), 1, Scalar(255,0,0), 1, CV_AA);//*/

		//Determine if enough corners survived------------------------------------------------------------------
		if (corners.size() < maxCorners*0.5)
		{
			init = true;
			//cout << "Corners are too scattered. Reinitialize  " << corners.size();
		} else
		{
			features = corners;
			AveStdDev(corners,xmean,ymean,xstd,ystd);
			handloc = Point(xmean,ymean);
			init = false;
		}

	} else
	{
		corners.reserve(maxCorners);
		corners.clear();
		cvtColor(frame1, prev, CV_BGR2GRAY);
		bitwise_and(prev,prevMask,prev);
		corners = features;

		//Eliminate corners not within neighborhood of previous solution----------------------------------------
		for (int i=0; i<corners.size(); i++)
		{
			if (sqrt(pow(prevloc.x-corners[i].x,2) + pow(prevloc.y-corners[i].y,2)) > sRadius)
			{
				corners.erase(corners.begin()+i);	i--;
			}
		}

		//Determine if enough corners remained------------------------------------------------------------------
		if (corners.size() < mintrackcorners)
		{
			init = true;
			//cout << "Insufficient corners remained. Reinitialize";
			return;
		}

		//cout << corners.size() << "\t";

		for (int i=0; i<corners.size(); i++)
		{
			corners[i].x = corners[i].x + initROI.x;
			corners[i].y = corners[i].y + initROI.y;
		}

		//Find corners in the next frame using Optical Flow
		features_found.reserve(maxCorners);
		feature_errors.reserve(maxCorners);
		corners_b.reserve(maxCorners);
		cvtColor(frame2, next, CV_BGR2GRAY);
		bitwise_and(next,nextMask,next);
		calcOpticalFlowPyrLK(prev, next, corners, corners_b, features_found, feature_errors, cvSize( 10, 10 ), 5, cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3 ), 0);

		//Label large motion vectors----------------------------------------------------------------------------
		for (int i=0; i<corners.size(); i++)
		{
			float dev = (float)sqrt(pow((corners[i].x - corners_b[i].x),2) + pow((corners[i].y - corners_b[i].y),2));
			if (dev > maxDev)
				features_found[i] = 0;
		}


		//*calculate mean motion vector--------------------------------------------------------------------------
		float meandev=0;
		int count=0;
		for (int i=0; i<corners.size(); i++)
		{
			if (features_found[i] != 0)
			{
				meandev = (float)(meandev + sqrt(pow((corners[i].x - corners_b[i].x),2) + pow((corners[i].y - corners_b[i].y),2)));
				count++;
			}
		}
		meandev = meandev/count;

		//Label small motion vectors------------------------------------------------------------------------
		if(meandev > 10)
		{
			for (int i=0; i<corners.size(); i++)
			{
				if (features_found[i] == 0)
				{
					float dev = (float)sqrt(pow((corners[i].x - corners_b[i].x),2) + pow((corners[i].y - corners_b[i].y),2));
					if(dev < 20)
						features_found[i] = 0;
				}
			}
		}//*/

		//Eliminate features not found, large motion vectors, small motion vectors-------------------------------
		for (int i=0; i<corners.size(); i++)
		{
			if (features_found[i] == 0)
			{
				corners_b.erase(corners_b.begin()+i);
				corners.erase(corners.begin()+i);
				features_found.erase(features_found.begin()+i);
				i--;
			}

		}
		//Determine if enough corners survived------------------------------------------------------------------
		if (corners_b.size() < 5)
		{
			init = true;
			//cout << "Only " << corners.size() << " corners remained. Reinitialize";
			return;
		}

		/*Draw Motion Vectors----------------------------------------------------------------------------------
		for(int i=0; i<corners_b.size(); i++)
		{
			Point p = Point(corners[i].x, corners[i].y);
			Point q = Point(corners_b[i].x, corners_b[i].y);

			double angle = atan2(p.y-q.y, p.x-q.x);
			double hypotenuse = sqrt(pow((p.y-q.y),2) + pow((p.x-q.x),2));

			//Lengthening the arrow by a factor of three, draw main arrow 
			q.x = (p.x - 1*hypotenuse*cos(angle));
			q.y = (p.y - 1*hypotenuse*sin(angle));
			line(Results, p, q, Scalar(255,255,255), 1, CV_AA, 0);
			
			//Draw arrowhead
			p.x = (q.x + 5*cos(angle + pi/4));
			p.y = (q.y + 5*sin(angle + pi/4));
			line(Results, p, q, Scalar(255,255,255), 1, CV_AA, 0);
			p.x = (q.x + 5*cos(angle - pi/4));
			p.y = (q.y + 5*sin(angle - pi/4));
			line(Results, p, q, Scalar(255,255,255), 1, CV_AA, 0);
		}//*/

		AveStdDev(corners,xmean,ymean,xstd,ystd);
		Point temp1 = Point(xmean,ymean);
		AveStdDev(corners_b,xmean,ymean,xstd,ystd);
		Point temp2 = Point(xmean,ymean);

		handloc.x = prevloc.x+(temp2.x-temp1.x);
		handloc.y = prevloc.y+(temp2.y-temp1.y);


		//Draw Results------------------------------------------------------------------------------------------
		circle(Results, Point(handloc.x+ROI.x,handloc.y+ROI.y), sRadius, Scalar(0,0,255), 5, 0);
		circle(Results, Point(handloc.x+ROI.x,handloc.y+ROI.y), 5, Scalar(0,0,255), -1, 0);

		//Assign tracked features to primary containter
		features = corners_b;

		//*Remove features too close to other features-----------------------------------------------------------
		Mat featuremap(480,640,CV_8UC1,Scalar(0));
		Mat featuremask(480,640,CV_8UC1,Scalar(0));
		Mat maskedfeatures(480,640,CV_8UC1,Scalar(0));

		for (int i=0; i<features.size(); i++)
			featuremap.at<uchar>(features[i].y,features[i].x) = 255;

		for (int i=0; i<features.size(); i++)
		{
			rectangle(featuremask,Point(0,0),Point(ROI.width,ROI.height),Scalar(0),-1,8,0);

			circle(featuremask,Point(features[i].x,features[i].y),10,Scalar(255),-1,8,0);
			bitwise_and(featuremap,featuremask,maskedfeatures);
			Scalar s = sum(maskedfeatures)[0]/255;
			if(s.val[0] > 1)
			{
				featuremap.at<uchar>(features[i].y,features[i].x) = 0;
				features.erase(features.begin()+i);
				i--;
			}
		}//*/

		corners.clear();
		corners.reserve(maxCorners);

		//REPLENISH FEATURES------------------------------------------------------------------------------------
		cvtColor(frame2(ROI), next, CV_BGR2GRAY);
		goodFeaturesToTrack(next,corners,maxCorners,qualityLevel,minDistance,cv::Mat(),blockSize,useHarrisDetector);

		for (int i=0; i<corners.size(); i++)
		{
			if (corners[i].x == ROI.x || corners[i].x == ROI.x+ROI.width || corners[i].y == ROI.y || corners[i].y == ROI.y+ROI.height)
			{
				corners.erase(corners.begin()+i);	i--;
			}
		}

		//*Eliminate corners not within neighborhood of solution---------------------------------------------
		for (int i=0; i<corners.size(); i++)
		{
			if (sqrt(pow(handloc.x-corners[i].x,2) + pow(handloc.y-corners[i].y,2)) > sRadius)
			{
				corners.erase(corners.begin()+i);
				i--;
			}
		}//*/

		Mat featuremap2(ROI.height,ROI.width,CV_8UC1,Scalar(0));
		Mat featuremask2(ROI.height,ROI.width,CV_8UC1,Scalar(0));
		Mat maskedfeatures2(ROI.height,ROI.width,CV_8UC1,Scalar(0));


		for (int i=0; i<features.size(); i++)
		{
			features[i].x = features[i].x - ROI.x;
			features[i].y = features[i].y - ROI.y;
		}

		for(int i=0; i<features.size(); i++)
			featuremap2.at<uchar>(features[i].y,features[i].x) = 255;

		//*Append additional corners
		for (int i=0; i<corners.size(); i++)
		{	
			rectangle(featuremask2,Point(0,0),Point(ROI.width,ROI.height),Scalar(0),-1,8,0);
			circle(featuremask2,corners[i],5,Scalar(255),-1,8,0);
			bitwise_and(featuremap2,featuremask2,maskedfeatures2);
			if(sum(maskedfeatures2).val[0]/255 < 2)
			{
				if(features.size() < maxCorners)
				{
					features.push_back(corners[i]);
					featuremap2.at<uchar>(corners[i].y,corners[i].x) = 255;
				}
			}
		}//*/

		//cout << features.size();

		features.clear();
		features.reserve(maxCorners);
		features = corners;

		tracked = true;
	}
	

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////DEFINE ROI///////////////////////////////////////////////////
void defineROI(Rect &ROI, Rect leftBreast, Rect rightBreast)
{
	if(s == 0)
	{
		ROI.x = leftBreast.x;
		ROI.y = leftBreast.y;
		ROI.width = 1.25*leftBreast.width;
		ROI.height = 1.1*leftBreast.height;
	} else if(s == 1)
	{
		ROI.x = rightBreast.x - 0.25*rightBreast.width;
		ROI.y = rightBreast.y;
		ROI.width = 1.25*rightBreast.width;
		ROI.height = 1.25*rightBreast.height;
	}

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////LEAST SQUARES LINEAR REGRESSION/////////////////////////////////////////////
void regression(Mat &buffer, float &A, float &B)
{
	float g1,g2,g3,g4,h1,h2;

	g1 = 40425;
	g2 = 1225;
	g3 = g2;
	g4 = 50;

	h1 = 0;
	h2 = 0;
	for(int count=0; count<50; count++)
	{
		h1 = h1 + count*buffer.at<float>(count);
		h2 = h2 + buffer.at<float>(count);
	}

	float r1, r2, r3, r4;

	r1 = (float)(h1/g1);
	r2 = (float)(h2/g3);
	r3 = (float)(g2/g1);
	r4 = (float)(g4/g3);

	B = (float)((r1-r2)/(r3-r4));
	A = (float)(r1 - B*r3);

	/*
	cout << endl << "h1: " << h1;
	cout << endl << "h2: " << h2;
	cout << endl << "r1: " << r1;
	cout << endl << "r2: " << r2;
	cout << endl << "r3: " << r3;
	cout << endl << "r4: " << r4;
	cout << endl << "B:  " << B;
	cout << endl << "A:  " << A;
	cin.get();
	//*/

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////PALPATION DETECTION/////////////////////////////////////////////////
void detectPalpation(Mat &xBuffer, Mat &yBuffer, Point handloc, bool tracked, Mat &Results, Rect currentBlock, Rect ROI)
{

	//Slide buffer elements
	for(int count=49; count>0; count--)
	{
		xBuffer.at<float>(count) = xBuffer.at<float>(count-1);
		yBuffer.at<float>(count) = yBuffer.at<float>(count-1);
	}

	//Insert newest element
	xBuffer.at<float>(0) = handloc.x;
	yBuffer.at<float>(0) = handloc.y;

	//Draw buffer elements
	for(int count=49; count>0; count--)
	{
		if(xBuffer.at<float>(count) == 0)
			continue;
	}
	
	//Clear buffer
	if(tracked==false)
	{
		for(int count=49; count>=0; count--)
		{
			xBuffer.at<float>(count) = 0;
			yBuffer.at<float>(count) = 0;
		}
	}

	//Linear Regression
	float xA, xB, yA, yB;
	regression(xBuffer,xA,xB);
	regression(yBuffer,yA,yB);

	//Draw graph
	Mat xGraph(640,300,CV_8UC1,Scalar(255));
	Mat yGraph(480,300,CV_8UC1,Scalar(255));	

	for(int count=49; count>0; count--)
	{
		if(xBuffer.at<float>(count) == 0)
			continue;
		line(xGraph,Point((count*6),xBuffer.at<float>(count)),Point((count-1)*6,xBuffer.at<float>(count-1)),Scalar(10),2,8,0);
		line(yGraph,Point((count*6),yBuffer.at<float>(count)),Point((count-1)*6,yBuffer.at<float>(count-1)),Scalar(10),2,8,0);
	}

	line(xGraph,Point(0,(xA*0+xB)),Point(300,(xA*50+xB)),Scalar(50),2,8,0);
	line(yGraph,Point(0,(yA*0+yB)),Point(300,(yA*50+yB)),Scalar(50),2,8,0);


	//Count cycles
	int xCycles = 0;
	int xMid = (xA*25+xB);
	int xlastCycleIndex = 0;
	int xfirstCycleIndex = 0;
	
	for(int count=0; count<49; count++)
	{
		if(xBuffer.at<float>(count) < xMid)
		{
			if(xBuffer.at<float>(count+1) > xMid)
			{
				xCycles++;
				xlastCycleIndex = count;
				if(xCycles == 1)
					xfirstCycleIndex = count;
			}
		}
	}

	//Count cycles
	int yCycles = 0;
	int yMid = (yA*25+yB);
	int ylastCycleIndex = 0;
	int yfirstCycleIndex = 0;

	for(int count=0; count<49; count++)
	{
		if(yBuffer.at<float>(count) < yMid)
		{
			if(yBuffer.at<float>(count+1) > yMid)
			{
				yCycles++;
				ylastCycleIndex = count;
				if(yCycles == 1)
					yfirstCycleIndex = count;
			}
		}
	}

	//Detect palpation
	float slopeThresh = 0.3;
	int cycleThresh = 3;
	int cycleMax = 4;
	bool palpCriteria[2][6] = {{0,0,0,0,0,0},{0,0,0,0,0,0}};
	bool palpDetect = false;
	bool palpEvaluate = false;

	float minimumSize = (float)(0.3*25*25*currentBlock.width);
	float maximumSize = (float)(0.7*25*25*currentBlock.width);

	Mat xBufferTemp;
	xBuffer.copyTo(xBufferTemp);
	xBufferTemp = xBufferTemp - Scalar(xMid);

	float palpSize = 0;
	for(int count=0; count<49; count++)
		palpSize = palpSize + pow(xBufferTemp.at<float>(count),2);

	cout << "palpSize = " << palpSize << "\tminSize = " << minimumSize << "\tmaxSize = " << maximumSize;

	//Palpation Criteria:
	// #0  ->  minimum number of cycles
	// #1  ->  slope
	// #2  ->  location
	// #3  ->  max number of cycles (upper speed limit)
	// #4  ->  minimum palpation size
	// #5  ->  maximum palpation size

	if(xCycles >= cycleThresh)
		palpCriteria[0][0] = true;

	if(xA < slopeThresh && xA > -slopeThresh)
		palpCriteria[0][1] = true;

	if(xMid > currentBlock.x-ROI.x && xMid < currentBlock.x-ROI.x+currentBlock.width)
		palpCriteria[0][2] = true;

	if(xCycles <= cycleMax)
		palpCriteria[0][3] = true;

	//if(palpSize > minimumSize)
		palpCriteria[0][4] = true;

	//if(palpSize < maximumSize)
		palpCriteria[0][5] = true;




	
	palpDetect		= palpCriteria[0][0] && palpCriteria[0][1];
	palpEvaluate	= palpCriteria[0][2] && palpCriteria[0][3] && palpCriteria[0][4] && palpCriteria[0][5];

	//circle(Results,Point(xMid+ROI.x,yMid+ROI.y),4,Scalar(0,255,0),4,8,0);

	//*
	if(palpDetect)
	{
		for(int count=xfirstCycleIndex; count<49; count++)
		{
			xBuffer.at<float>(count) = 0;
			yBuffer.at<float>(count) = 0;
		}

		if(palpEvaluate)
		{
			rectangle(Results,currentBlock,Scalar(0,255,0),1,8,0);
			Results(currentBlock) = Results(currentBlock) + Scalar(0,150,0);
			pCount++;
			sprintf(message2,"");

			PlaySound(TEXT("Audio/Ding.wav"), NULL, SND_ASYNC);
		} else
		{
			if(palpCriteria[0][2] == 0)
			{
				sprintf(message2,"Wrong block! Please palpate the white block.");
				PlaySound(TEXT("Audio/Message_7.wav"), NULL, SND_ASYNC);
				rectangle(Results,currentBlock,Scalar(-200,-200,255),1,8,0);
				Results(currentBlock) = Results(currentBlock) + Scalar(0,0,150);
			} else if(palpCriteria[0][3] == 0)
			{
				sprintf(message2,"Too fast! Please palpate slower.");
				PlaySound(TEXT("Audio/Message_8.wav"), NULL, SND_ASYNC);
			} else if(palpCriteria[0][4] == 0)
			{
				sprintf(message2,"Palpation motion is too small.");
				PlaySound(TEXT("Audio/Message_9.wav"), NULL, SND_ASYNC);
			} else if(palpCriteria[0][5] == 0)
			{
				sprintf(message2,"Palpation motion is too large.");
				PlaySound(TEXT("Audio/Message_10.wav"), NULL, SND_ASYNC);
			}
		}
	}
	//*/
	


	Size dsize(320,480);
	resize(xGraph,xGraph,dsize);
	resize(yGraph,yGraph,dsize);

	imshow("xGraph",xGraph);
	imshow("yGraph",yGraph);

	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////MONITOR PALPATION//////////////////////////////////////////////////
void monitorPalpation(Mat &result, Rect &ROI)
{
	if(pCount>2)
	{
		blockStatus1[s][r][c] = true;
		r++;
		if(r==upperRows+lowerRows)
		{
			r=0;
			c++;

			if(c==innerCols+outerCols)
			{
				c=0;
				s++;

				if(s!=2)
				{
					pCount = 0;
				}
			}
		}

		pCount = 0;
	}

	//*
	for(int side=0; side<2; side++)
	{
		for(int col=0; col<innerCols+outerCols; col++)
		{
			for(int row=0; row<upperRows+lowerRows; row++)
			{
				ROI = Block[side][row][col];
				rectangle(result,ROI,Scalar(0,0,0),1,8,0);

				if(side==s && row==r && col==c)
					result(ROI) = result(ROI) + Scalar(100,100,100);
				else if(blockStatus1[side][row][col] == false)
					result(ROI) = result(ROI) + Scalar(-50,-50,-50);
				else
					result(ROI) = result(ROI) + Scalar(-50,50,-50);
			}
		}
	}

	//*/

	ROI = Block[s][r][c];


	return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////





////////////////////////////////////////////////MAIN FUNCTION///////////////////////////////////////////////////
int main(int argc, char* argv[])
{	
	Mat frame1, frame2, frame_output, frame_output_buffer;
	Point prevloc;
	vector<Point2f> features;	features.reserve(maxCorners);
	VideoCapture cap(0);

	Rect leftBreast, rightBreast;
	Rect leftNipple, rightNipple;
	
	initGUI();

	Mat xBuffer(50,1,CV_32FC1,Scalar(0));
	Mat yBuffer(50,1,CV_32FC1,Scalar(0));

	//Iterate through video frames------------------------------------------------------------------------------
	for( ; ; current_frame = current_frame + 1)
	{
		cap.set(CV_CAP_PROP_POS_FRAMES,current_frame);
		bool bSuccess = cap.read(frame2);
		if(!bSuccess)	break;
		Size dsize(640,480);
		flip(frame2,frame2,1);
		resize(frame2,frame2,dsize);
		frame2.copyTo(frame_output);
		cout << endl << current_frame << "\t";

		Point handloc;

		//temp_frame.copyTo(frame_output);
		
		//*
		if(program_state == 0)
		{
			sprintf(message1,"");
			sprintf(message2,"");
			default_frame.copyTo(frame_output);
		} else if(program_state == 1)
		{
			sprintf(message1,"Click the play button to begin.");
			sprintf(message2,"");
			delineated = false;

		} else if(program_state == 2)
		{
			//delineated = true;
			if(delineated == true)
			{
				destroyWindow("Breast Parameters");
				destroyWindow("Nipple Parameters");
				divideBreastArea(frame2,frame_output,leftBreast,leftNipple,rightBreast,rightNipple);
				program_state = 3;
			} else
			{
				//delineate(frame2,frame_output);
				sprintf(message1,"Adjust the bounding box using the sliders.");
				sprintf(message2,"Click the play button to continue.");
				annotate(frame2,leftBreast,rightBreast,leftNipple,rightNipple,delineated,frame_output);

				//locateNipple(leftBreast,frame2,leftNipple,"left");
				//locateNipple(rightBreast,frame2,rightNipple,"right");
			}

			

		} else if(program_state == 3)
		{

			//*HAND TRACKING CODE-----------------------------------------------------------------------------------
			bool tracked = false;
			Rect ROI;
			defineROI(ROI,leftBreast,rightBreast);
			//ROI = leftBreast;

			if(init == true)
			{
				sprintf(message1,"Place your hand over the yellow box.");
				sprintf(message2,"");

			} else
			{
				sprintf(message1,"Palpate the white block.");
			}

			HandTracker2(frame1,frame2,ROI,prevloc,handloc,features,tracked,frame_output);

			Rect currentBlock;
			monitorPalpation(frame_output,currentBlock);

			//*PALPATION DETECTION----------------------------------------------------------------------------------
			detectPalpation(xBuffer,yBuffer,handloc,tracked,frame_output,currentBlock,ROI);


			//Buffer frame and results------------------------------------------------------------------------------
			frame2.copyTo(frame1);
			prevloc = handloc;
		} else if(program_state == 4)
		{
			sprintf(message1,"Click the play button to continue.");
			sprintf(message2,"");
			frame_output_buffer.copyTo(frame_output);
		} else if(program_state == 5)
		{
			sprintf(message1,"BSE is complete.");
			sprintf(message2,"");
			frame_output_buffer.copyTo(frame_output);
		}
		//*/

		displayGUI(frame_output);
		frame_output.copyTo(frame_output_buffer);

		/*Write frames to file
		char filename[20];
		sprintf(filename,"Images/%d.jpg",current_frame);q
		imwrite(filename,frame_output);
		//*/

		//*Manual exit mechanism---------------------------------------------------------------------------------
		char button = waitKey(5);
		if(button == 'r')	init = true;
		else if(button == 'i')	clicked = false;
		else if(button == 'q')	break;
		else if(button == 'c')	pCount++;
		else if(button == 'v')	pCount--;
		//*/
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
