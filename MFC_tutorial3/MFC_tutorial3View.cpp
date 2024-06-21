
// MFC_tutorial3View.cpp : implementation of the CMFCtutorial3View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFC_tutorial3.h"
#endif

#include "MFC_tutorial3Doc.h"
#include "MFC_tutorial3View.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <vector>
#include <iostream>

#include <utility>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCtutorial3View

IMPLEMENT_DYNCREATE(CMFCtutorial3View, CView)

BEGIN_MESSAGE_MAP(CMFCtutorial3View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCtutorial3View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_OPEN, &CMFCtutorial3View::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CMFCtutorial3View::OnFileSave)
	ON_COMMAND(ID_TOOLS_ROTATE45, &CMFCtutorial3View::OnToolsRotate45)
	ON_COMMAND(ID_TOOLS_ROTATE, &CMFCtutorial3View::OnToolsRotate)
	ON_COMMAND(ID_TOOLS_ROTATE90, &CMFCtutorial3View::OnToolsRotate90)
	ON_COMMAND(ID_TOOLS_ROTATE32774, &CMFCtutorial3View::OnToolsRotate32774)
	ON_COMMAND(ID_TOOLS_SHARPEN, &CMFCtutorial3View::OnToolsSharpen)
	ON_COMMAND(ID_Menu, &CMFCtutorial3View::OnMenu)
	ON_COMMAND(ID_TOOLS_ZOOMOUT, &CMFCtutorial3View::OnToolsZoomout)
	ON_COMMAND(ID_EDIT_UNDO, &CMFCtutorial3View::OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO32778, &CMFCtutorial3View::OnEditRedo32778)
	ON_COMMAND(ID_TOOLS_EDGEDETECTION, &CMFCtutorial3View::OnToolsEdgedetection)
	ON_COMMAND(ID_TOOLS_FLIPHORIZONTALLY, &CMFCtutorial3View::OnToolsFliphorizontally)
	ON_COMMAND(ID_TOOLS_FLIPVERTICALLY, &CMFCtutorial3View::OnToolsFlipvertically)
	ON_COMMAND(ID_TOOLS_LAPLACIAN, &CMFCtutorial3View::OnToolsLaplacian)
END_MESSAGE_MAP()

// CMFCtutorial3View construction/destruction

CMFCtutorial3View::CMFCtutorial3View() noexcept
{
	// TODO: add construction code here
}

CMFCtutorial3View::~CMFCtutorial3View()
{
}

BOOL CMFCtutorial3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMFCtutorial3View drawing

void CMFCtutorial3View::OnDraw(CDC* /*pDC*/)
{
	CMFCtutorial3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CMFCtutorial3View printing


void CMFCtutorial3View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCtutorial3View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFCtutorial3View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFCtutorial3View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMFCtutorial3View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCtutorial3View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCtutorial3View diagnostics

#ifdef _DEBUG
void CMFCtutorial3View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCtutorial3View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCtutorial3Doc* CMFCtutorial3View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCtutorial3Doc)));
	return (CMFCtutorial3Doc*)m_pDocument;
}
#endif //_DEBUG


// CMFCtutorial3View message handlers
cv::Mat ImageMat;

std::vector<cv::Mat> undoRedo_vector;
std::vector<std::pair<std::string, double>> opp;

cv::Mat zoomedImg;
//bool zoomedImgSentinel = true;
//int zoomedImgIdx = 0;
double zoomedImgScale = 1.0;
//int undoRedo = 0;

int undoRedo_vector_i = 0;
cv::Mat currentImageDisplayed;
double angle = 0.0;
HWND hParent;
CString CstrWindowname{ "Displaywindow" };
std::string strWindowname{ "Displaywindow" };

void RotateImage(double angle_rot, bool direction, std::string windowName) {

	if (ImageMat.empty()) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	if (direction) {
		angle += angle_rot;
	}
	else {
		angle += angle_rot;
	}

	// get rotation matrix for rotating the image around its center in pixel coordinates
	cv::Point2f center((currentImageDisplayed.cols - 1) / 2.0, (currentImageDisplayed.rows - 1) / 2.0);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
	// determine bounding rectangle, center not relevant
	cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), currentImageDisplayed.size(), angle).boundingRect2f();
	// adjust transformation matrix
	rot.at<double>(0, 2) += bbox.width / 2.0 - currentImageDisplayed.cols / 2.0;
	rot.at<double>(1, 2) += bbox.height / 2.0 - currentImageDisplayed.rows / 2.0;

	cv::Mat dst;
	cv::warpAffine(currentImageDisplayed, dst, rot, bbox.size());
	//cv::imshow("Displaywindow", dst);
	currentImageDisplayed = dst;

	if (undoRedo_vector_i == (undoRedo_vector.size() - 1)) {
		undoRedo_vector.push_back(currentImageDisplayed);
		opp.push_back({ "rot", angle_rot });
		undoRedo_vector_i++;
	}
	else {
		undoRedo_vector[++undoRedo_vector_i] = currentImageDisplayed;
		opp[undoRedo_vector_i] = { "rot", angle_rot };
		undoRedo_vector.erase(undoRedo_vector.begin() + undoRedo_vector_i + 1, undoRedo_vector.end());
		opp.erase(opp.begin() + undoRedo_vector_i + 1, opp.end());
	}

	//if (!zoomedImgSentinel) {
	//	//zoomedImg = currentImageDisplayed;
	//	cv::resize(currentImageDisplayed, currentImageDisplayed, cv::Size(zoomedImg.cols * zoomedImgScale, zoomedImg.rows * zoomedImgScale));
	//}
	/*undoRedo_vector.push_back(currentImageDisplayed);
	undoRedo_vector_i++;*/
	cv::imshow(windowName, currentImageDisplayed);
}

cv::Mat RotateHelper(cv::Mat scr, double angle_rot, bool direction, std::string windowName) {
	/*if (firstTimeRot) {
		if (!currentImageDisplayed.empty()) {
			beforeRot = currentImageDisplayed;
		}
		firstTimeRot = false;
	}*/
	

	// get rotation matrix for rotating the image around its center in pixel coordinates
	cv::Point2f center((scr.cols - 1) / 2.0, (scr.rows - 1) / 2.0);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle_rot, 1.0);
	// determine bounding rectangle, center not relevant
	cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), scr.size(), angle_rot).boundingRect2f();
	// adjust transformation matrix
	rot.at<double>(0, 2) += bbox.width / 2.0 - scr.cols / 2.0;
	rot.at<double>(1, 2) += bbox.height / 2.0 - scr.rows / 2.0;

	cv::Mat dst;
	cv::warpAffine(scr, dst, rot, bbox.size());
	//cv::imshow("Displaywindow", dst);
	return dst;

}

void CMFCtutorial3View::OnFileOpen()
{
	// TODO: Add your command handler code here
	const TCHAR szFilter[] = _T("All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("jpeg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		CString sFilePath = dlg.GetPathName();
		CT2CA pszConvertedAnsiString(sFilePath);
		// construct a std::string using the LPCSTR input
		std::string strStd(pszConvertedAnsiString);

		cv::Mat I = cv::imread(strStd, cv::IMREAD_UNCHANGED);

		if (I.channels() == 4) {
			for (int i = 0; i < I.rows; i++)
			{
				for (int j = 0; j < I.cols; j++)
				{
					cv::Vec4b bgrPixel = I.at<cv::Vec4b>(i, j);
					if (bgrPixel[3] < 10) {
						cv::Vec4b temp{ 255, 255, 255, 255 };
						I.at<cv::Vec4b>(i, j) = temp;
					}
					// do something with BGR values...
				}
			}
		}

		if (I.empty())
		{
			AfxMessageBox(_T("Could not read image!"), MB_OK | MB_ICONSTOP);

		}
		else {
			//cv::startWindowThread();
			std::string windowName{ "Displaywindow" };
			cv::namedWindow("Displaywindow");// Create a window for display.
			try {
				cv::imshow("Displaywindow", I);
				ImageMat = I;
				currentImageDisplayed = I;
				angle = 0.0;
				undoRedo_vector.clear();
				undoRedo_vector.push_back(I);
				opp.clear();
				opp.push_back({ "init", 0.0 });
				undoRedo_vector_i = 0;
				
				cv::setWindowProperty("Displaywindow", cv::WND_PROP_TOPMOST, 1);

				//namedWindow(windowname, WINDOW_AUTOSIZE);

				////// This will work on opencv 4.X //////
				CstrWindowname = (CString)windowName.c_str();

				//CString CstrWindowname = windowName.data();

				hParent = (HWND)FindWindow(NULL, CstrWindowname);
			}
			catch (std::exception& exc) {
				AfxMessageBox(_T("Could not read image!"), MB_OK | MB_ICONSTOP);

			}
		}
	}
}


void CMFCtutorial3View::OnFileSave()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("Cannot save image. No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	const TCHAR szFilter[] = _T("Picture Files (*.jpeg, *.png)|*.jpeg;*.png;|Jpeg (*.jpeg)|*.jpeg;|Png (*.png)|*.png;|");
	/*_T("Jpeg Files (*.jpeg)|*.jpeg|")
		_T("Png Files (*.png)|*.png|");*/
	CFileDialog dlg(FALSE, _T("jpeg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		CString sFilePath = dlg.GetPathName();
		CT2CA pszConvertedAnsiString(sFilePath);
		// construct a std::string using the LPCSTR input
		std::string strStd(pszConvertedAnsiString);



		if (currentImageDisplayed.empty())
		{
			AfxMessageBox(_T("Could not write image!"), MB_OK | MB_ICONSTOP);

		}
		else {
			cv::imwrite(strStd, currentImageDisplayed);
			AfxMessageBox(_T("Image saved succesfully!"), MB_OK | MB_ICONINFORMATION);

		}
	}
}


void CMFCtutorial3View::OnToolsRotate45()
{
	// TODO: Add your command handler code here
	if (ImageMat.empty()) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}
	currentImageDisplayed = RotateHelper(currentImageDisplayed, -angle, true, "Displaywindow");
	RotateImage(-45.0, true, "Displaywindow");
}


void CMFCtutorial3View::OnToolsRotate()
{
	// TODO: Add your command handler code here
	if (ImageMat.empty()) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}
	currentImageDisplayed = RotateHelper(currentImageDisplayed, -angle, true, "Displaywindow");
	RotateImage(45.0, false, "Displaywindow");
}


void CMFCtutorial3View::OnToolsRotate90()
{
	// TODO: Add your command handler code here
	if (ImageMat.empty()) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}
	currentImageDisplayed = RotateHelper(currentImageDisplayed, -angle, true, "Displaywindow");
	RotateImage(-90.0, true, "Displaywindow");
}


void CMFCtutorial3View::OnToolsRotate32774()
{
	// TODO: Add your command handler code here
	if (ImageMat.empty()) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}
	currentImageDisplayed = RotateHelper(currentImageDisplayed, -angle, true, "Displaywindow");
	RotateImage(90.0, false, "Displaywindow");

}


void CMFCtutorial3View::OnToolsSharpen()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("Cannot sharpen image. No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}
	// TODO: Add your command handler code here
	cv::Mat afterSharpen;
	cv::GaussianBlur(currentImageDisplayed, afterSharpen, cv::Size(0, 0), 3);
	cv::addWeighted(currentImageDisplayed, 1.5, afterSharpen, -0.5, 2.0/*0*/, afterSharpen);

	currentImageDisplayed = afterSharpen;

	if (undoRedo_vector_i == (undoRedo_vector.size() - 1)) {
		undoRedo_vector.push_back(currentImageDisplayed);
		opp.push_back({ "filter", 0.0 });
		undoRedo_vector_i++;
	}
	else {
		undoRedo_vector[++undoRedo_vector_i] = currentImageDisplayed;
		opp[undoRedo_vector_i] = { "filter", 0.0 };
		undoRedo_vector.erase(undoRedo_vector.begin() + undoRedo_vector_i + 1, undoRedo_vector.end());
		opp.erase(opp.begin() + undoRedo_vector_i + 1, opp.end());

	}
	CT2CA pszConvertedAnsiString(CstrWindowname);
	// construct a std::string using the LPCSTR input
	std::string strStd(pszConvertedAnsiString);

	cv::imshow(strWindowname, currentImageDisplayed);
}


void CMFCtutorial3View::OnMenu()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("No image displayed, cannot zoom"), MB_OK | MB_ICONSTOP);
		return;
	}

	zoomedImg = currentImageDisplayed;

	zoomedImgScale = 1.0 / 1.10;

	cv::resize(zoomedImg, currentImageDisplayed, cv::Size(zoomedImg.cols * zoomedImgScale, zoomedImg.rows * zoomedImgScale), cv::INTER_AREA);
	//interpolation = cv2.INTER_AREA
	//cv::resize(zoomedImg, currentImageDisplayed, cv::Size(),  zoomedImgScale, zoomedImgScale, cv::INTER_CUBIC);

	if (undoRedo_vector_i == (undoRedo_vector.size() - 1)) {
		undoRedo_vector.push_back(currentImageDisplayed);
		opp.push_back({ "zoom_in", 1.05 });
		undoRedo_vector_i++;
	}
	else {
		undoRedo_vector[++undoRedo_vector_i] = currentImageDisplayed;
		opp[undoRedo_vector_i] = { "zoom_in", 1.05 };
		undoRedo_vector.erase(undoRedo_vector.begin() + undoRedo_vector_i + 1, undoRedo_vector.end());
		opp.erase(opp.begin() + undoRedo_vector_i + 1, opp.end());
	}

	cv::imshow(strWindowname, currentImageDisplayed);
}


void CMFCtutorial3View::OnToolsZoomout()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("No image displayed, cannot zoom"), MB_OK | MB_ICONSTOP);
		return;
	}

	zoomedImg = currentImageDisplayed;
		
	zoomedImgScale = 1.10;

	cv::resize(zoomedImg, currentImageDisplayed, cv::Size(zoomedImg.cols * zoomedImgScale, zoomedImg.rows * zoomedImgScale), cv::INTER_CUBIC);

	if (undoRedo_vector_i == (undoRedo_vector.size() - 1)) {
		undoRedo_vector.push_back(currentImageDisplayed);
		opp.push_back({ "zoom_out", 1.05 });
		undoRedo_vector_i++;
	}
	else {
		undoRedo_vector[++undoRedo_vector_i] = currentImageDisplayed;
		opp[undoRedo_vector_i] = { "zoom_out", 1.05 };
		undoRedo_vector.erase(undoRedo_vector.begin() + undoRedo_vector_i + 1, undoRedo_vector.end());
		opp.erase(opp.begin() + undoRedo_vector_i + 1, opp.end());

	}

	cv::imshow(strWindowname, currentImageDisplayed);
}


void CMFCtutorial3View::OnEditUndo()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("No image displayed, cannot undo"), MB_OK | MB_ICONSTOP);
		return;
	}
	cv::Mat temp;

	if (undoRedo_vector_i > 0) {

		std::string opperation = opp[undoRedo_vector_i].first;
		if (!opperation.compare("rot")) {
			angle -= opp[undoRedo_vector_i].second;
		}


		undoRedo_vector_i--;

		cv::Mat source = undoRedo_vector.at(undoRedo_vector_i);
		//opperation = opp[undoRedo_vector_i].first;
		temp = source;

			//zoomedImg = currentImageDisplayed;
			if (!opperation.compare("zoom_in")) {
				zoomedImgScale = 1.0;
				cv::resize(source, temp, cv::Size(source.cols, source.rows));
			}
			else if (!opperation.compare("zoom_out")) {
				zoomedImgScale = 1.0;
				cv::resize(source, temp, cv::Size(source.cols, source.rows));
			}
			else {
				//cv::resize(source, temp, cv::Size(zoomedImg.cols * zoomedImgScale, zoomedImg.rows * zoomedImgScale));
			}

	}
	else if (undoRedo_vector_i <= -1) {
		undoRedo_vector_i = -1;
		return;
	}
	else {
		temp = undoRedo_vector.at(undoRedo_vector_i);
	}

	//



	cv::imshow(strWindowname, temp);
	currentImageDisplayed = temp;
}


void CMFCtutorial3View::OnEditRedo32778()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("No image displayed, cannot redo"), MB_OK | MB_ICONSTOP);
		return;
	}
	cv::Mat temp;

	if (undoRedo_vector.size() == 0) {
		return;
	}
	else if (undoRedo_vector_i < undoRedo_vector.size() - 1) {
		undoRedo_vector_i++;

		std::string opperation = opp[undoRedo_vector_i].first;
		if (!opperation.compare("rot")) {
			angle += opp[undoRedo_vector_i].second;
		}
		//else if (!opperation.compare("zoom_in")) {

		//}
		//else if (!opperation.compare("zoom_out")) {

		//}
		//else {
		//}

		cv::Mat source = undoRedo_vector.at(undoRedo_vector_i);
		temp = source;
		/*if (!zoomedImgSentinel) */{
			//zoomedImg = currentImageDisplayed;
			//std::string opperation = opp[undoRedo_vector_i].first;
			if (!opperation.compare("zoom_in")) {
				zoomedImgScale = 1.0;
				cv::resize(source, temp, cv::Size(source.cols, source.rows));
			}
			else if (!opperation.compare("zoom_out")) {
				zoomedImgScale = 1.0;
				cv::resize(source, temp, cv::Size(source.cols, source.rows));
			}
			else {
				//cv::resize(source, temp, cv::Size(zoomedImg.cols * zoomedImgScale, zoomedImg.rows * zoomedImgScale));
			}
		}

	}
	if (undoRedo_vector_i > undoRedo_vector.size() - 1) {
		undoRedo_vector_i = undoRedo_vector.size() - 1;
		return;
	}
	else {
		temp = undoRedo_vector.at(undoRedo_vector_i);
	}




	cv::imshow(strWindowname, temp);
	currentImageDisplayed = temp;
}


void CMFCtutorial3View::OnToolsEdgedetection()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("Cannot detect edges image. No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	cv::Mat edges; 
	cv::Mat currentImageDisplayed2;
	currentImageDisplayed.convertTo(currentImageDisplayed2, CV_8U);
	cv::Canny(currentImageDisplayed2, edges, 100, 200);
	currentImageDisplayed = edges;

	if (undoRedo_vector_i == (undoRedo_vector.size() - 1)) {
		undoRedo_vector.push_back(currentImageDisplayed);
		opp.push_back({ "filter", 1.0 });
		undoRedo_vector_i++;
	}
	else {
		undoRedo_vector[++undoRedo_vector_i] = currentImageDisplayed;
		opp[undoRedo_vector_i] = { "filter", 1.0 };
		undoRedo_vector.erase(undoRedo_vector.begin() + undoRedo_vector_i + 1, undoRedo_vector.end());
		opp.erase(opp.begin() + undoRedo_vector_i + 1, opp.end());

	}

	cv::imshow(strWindowname, currentImageDisplayed);
}


void CMFCtutorial3View::OnToolsFliphorizontally()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("Cannot flip image. No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	cv::Mat flip;
	cv::flip(currentImageDisplayed, flip , 0);
	currentImageDisplayed = flip;

	if (undoRedo_vector_i == (undoRedo_vector.size() - 1)) {
		undoRedo_vector.push_back(currentImageDisplayed);
		opp.push_back({ "filter", 2.0 });
		undoRedo_vector_i++;
	}
	else {
		undoRedo_vector[++undoRedo_vector_i] = currentImageDisplayed;
		opp[undoRedo_vector_i] = { "filter", 2.0 };
		undoRedo_vector.erase(undoRedo_vector.begin() + undoRedo_vector_i + 1, undoRedo_vector.end());
		opp.erase(opp.begin() + undoRedo_vector_i + 1, opp.end());

	}

	cv::imshow(strWindowname, currentImageDisplayed);

}


void CMFCtutorial3View::OnToolsFlipvertically()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("Cannot flip image. No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	cv::Mat flip;
	cv::flip(currentImageDisplayed, flip, 1);
	currentImageDisplayed = flip;

	if (undoRedo_vector_i == (undoRedo_vector.size() - 1)) {
		undoRedo_vector.push_back(currentImageDisplayed);
		opp.push_back({ "filter", 3.0 });
		undoRedo_vector_i++;
	}
	else {
		undoRedo_vector[++undoRedo_vector_i] = currentImageDisplayed;
		opp[undoRedo_vector_i] = { "filter", 3.0 };
		undoRedo_vector.erase(undoRedo_vector.begin() + undoRedo_vector_i + 1, undoRedo_vector.end());
		opp.erase(opp.begin() + undoRedo_vector_i + 1, opp.end());

	}

	cv::imshow(strWindowname, currentImageDisplayed);
}


void CMFCtutorial3View::OnToolsLaplacian()
{
	// TODO: Add your command handler code here
	hParent = (HWND)FindWindow(NULL, CstrWindowname);
	if (hParent == NULL) {
		AfxMessageBox(_T("Cannot laplacian image. No image displayed"), MB_OK | MB_ICONSTOP);
		return;
	}

	cv::Mat lap;
	cv::Laplacian(currentImageDisplayed, lap, CV_64F);
	currentImageDisplayed = lap;

	if (undoRedo_vector_i == (undoRedo_vector.size() - 1)) {
		undoRedo_vector.push_back(currentImageDisplayed);
		opp.push_back({ "filter", 4.0 });
		undoRedo_vector_i++;
	}
	else {
		undoRedo_vector[++undoRedo_vector_i] = currentImageDisplayed;
		opp[undoRedo_vector_i] = { "filter", 4.0 };
		undoRedo_vector.erase(undoRedo_vector.begin() + undoRedo_vector_i + 1, undoRedo_vector.end());
		opp.erase(opp.begin() + undoRedo_vector_i + 1, opp.end());

	}

	cv::imshow(strWindowname, currentImageDisplayed);

}
