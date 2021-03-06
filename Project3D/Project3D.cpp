#include "stdafx.h"
#include <fstream>
#include <string>
#include <iostream>
#include <glut.h>
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <wchar.h>
#include <math.h>

using namespace std;

static float sudut = 0.0;

typedef struct {
	float x;
	float y;
} point2D_t;

typedef struct
{
	float x, y, z;
} point3D_t;

typedef struct {
	float v[3];
} vector2D_t;

typedef struct {
	float v[4];
} vector3D_t;

typedef struct {
	float m[3][3];
} matrix2D_t;

typedef struct {
	float m[3][3];
} matrix3D_t;

typedef struct {
	int numOfVertices;
	long int pnt[32];
} face_t;

typedef struct {
	float r;
	float g;
	float b;
} color_t;

typedef struct {
	int numOfVertices;
	point3D_t pnt[600];
	int numOfColors;
	color_t col[10000];
	int numOfFaces;
	face_t fc[1200];
}object3D_t;

const int MAXSIZE = 1500;
point3D_t point[MAXSIZE]; face_t face[MAXSIZE]; color_t color[MAXSIZE];
vector3D_t vec[MAXSIZE], vecbuff[MAXSIZE], NormalVector;
point2D_t titik2D[MAXSIZE];

vector2D_t point2vector(point2D_t pnt) {
	vector2D_t vec;
	vec.v[0] = pnt.x;
	vec.v[1] = pnt.y;
	vec.v[2] = 1.;
	return vec;
}

point2D_t vector2point(vector2D_t vec) {
	point2D_t pnt;
	pnt.x = vec.v[0];
	pnt.y = vec.v[1];
	return pnt;
}

point2D_t vector2point2d(vector3D_t vec)
{
	point2D_t pnt;
	pnt.x = vec.v[0];
	pnt.y = vec.v[1];
	return pnt;
}

vector3D_t point2vector(point3D_t pnt)
{
	vector3D_t vec;
	vec.v[0] = pnt.x;
	vec.v[1] = pnt.y;
	vec.v[2] = pnt.z;
	vec.v[3] = 1.;
	return vec;
}

matrix3D_t createIdentity() {
	matrix3D_t rotate;
	rotate.m[0][0] = 0.0;
	rotate.m[0][1] = 0.0;
	rotate.m[0][2] = 0.0;
	rotate.m[1][0] = 0.0;
	rotate.m[1][1] = 0.0;
	rotate.m[1][2] = 0.0;
	rotate.m[2][0] = 0.0;
	rotate.m[2][1] = 0.0;
	rotate.m[2][1] = 0.0;
	return rotate;
}

matrix3D_t rotationX(float teta) {
	matrix3D_t rotate = createIdentity();
	rotate.m[0][0] = 1.0;
	rotate.m[0][1] = 0.0;
	rotate.m[0][2] = 0.0;
	rotate.m[1][0] = 0.0;
	rotate.m[1][1] = cos(teta / 57.3);
	rotate.m[1][2] = -sin(teta / 57.3);
	rotate.m[2][0] = 0.0;
	rotate.m[2][1] = sin(teta / 57.3);
	rotate.m[2][2] = cos(teta / 57.3);
	return rotate;
}

matrix3D_t rotationY(float teta) {
	matrix3D_t rotate = createIdentity();
	rotate.m[0][0] = cos(teta / 57.3);
	rotate.m[0][1] = 0.0;
	rotate.m[0][2] = sin(teta / 57.3);
	rotate.m[1][0] = 0.0;
	rotate.m[1][1] = 1.0;
	rotate.m[1][2] = 0.0;
	rotate.m[2][0] = -sin(teta / 57.3);
	rotate.m[2][1] = 0.0;
	rotate.m[2][2] = cos(teta / 57.3);
	return rotate;
}

matrix3D_t rotationZ(float teta) {
	matrix3D_t rotate = createIdentity();
	rotate.m[0][0] = cos(teta / 57.3);
	rotate.m[0][1] = -sin(teta / 57.3);
	rotate.m[0][2] = 0.0;
	rotate.m[1][0] = sin(teta / 57.3);
	rotate.m[1][1] = cos(teta / 57.3);
	rotate.m[1][2] = 0.0;
	rotate.m[2][0] = 0.0;
	rotate.m[2][1] = 0.0;
	rotate.m[2][2] = 1.0;
	return rotate;
}

vector3D_t operator +(vector3D_t a, vector3D_t b) {
	vector3D_t c;
	for (int i = 0; i<3; i++) {
		c.v[i] = a.v[i] + b.v[i];
	}
	return c;
}

vector3D_t operator -(vector3D_t a, vector3D_t b) {
	vector3D_t c;
	for (int i = 0; i<3; i++) {
		c.v[i] = a.v[i] - b.v[i];
	}
	return c;
}

vector3D_t operator *(matrix3D_t a, vector3D_t b) {
	vector3D_t c;
	for (int i = 0; i<3; i++) {
		c.v[i] = 0;
		for (int j = 0; j<3; j++) {
			c.v[i] += a.m[i][j] * b.v[j];
		}
	}
	return c;
}

vector3D_t operator ^ (vector3D_t a, vector3D_t b) {
	vector3D_t c; //c=a*b
	c.v[0] = a.v[1] * b.v[2] - a.v[2] * b.v[1];
	c.v[1] = a.v[2] * b.v[0] - a.v[0] * b.v[2];
	c.v[2] = a.v[0] * b.v[1] - a.v[1] * b.v[0];
	//c.v[3] = 1.;
	return c;
}

matrix2D_t operator *(matrix2D_t a, matrix2D_t b) {
	matrix2D_t c;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			c.m[i][j] = 0;
			for (int k = 0; k < 3; k++)
			{
				c.m[i][j] += a.m[i][k] * b.m[k][j];
			}
		}
	}
	return c;
}

matrix3D_t operator *(matrix3D_t a, matrix3D_t b) {
	matrix3D_t c;//c=a*b
	int i, j, k;
	for (i = 0; i<3; i++) for (j = 0; j<3; j++) {
		c.m[i][j] = 0;
		for (k = 0; k<3; k++)
			c.m[i][j] += a.m[i][k] * b.m[k][j];
	}
	return c;
}

vector2D_t operator *(matrix2D_t a, vector2D_t b) {
	vector2D_t c;
	for (int i = 0; i < 3; i++)
	{
		c.v[i] = 0;
		for (int j = 0; j < 3; j++)
		{
			c.v[i] += a.m[i][j] * b.v[j];
		}
	}
	return c;
}

void setColor(color_t col) {
	float r = col.r / 255;
	float g = col.g / 255;
	float b = col.b / 255;

	glColor3f(r, g, b);
}

void drawPolygon(point2D_t pnt[], int n) {
	int i;
	glBegin(GL_LINE_LOOP);
	for (i = 0; i<n; i++) {
		glVertex2i(pnt[i].x, pnt[i].y);
	}
	glEnd();
}

void drawPolygon(point2D_t pnt[], int n, color_t c) {
	int i;
	setColor(c);
	glBegin(GL_LINE_LOOP);
	for (i = 0; i<n; i++) {
		glVertex2i(pnt[i].x, pnt[i].y);
	}
	glEnd();
}

void drawPolyline(point2D_t pnt[], int n) {
	int i;
	glBegin(GL_LINE_STRIP);
	for (i = 0; i<n; i++) {
		glVertex2f(pnt[i].x, pnt[i].y);
	}
	glEnd();
}

void fillPolygon(point2D_t pnt[], int n, color_t color) {
	int i;
	setColor(color);
	glBegin(GL_POLYGON);
	for (i = 0; i < n; i++)
	{
		glVertex2f(pnt[i].x, pnt[i].y);
	}
	glEnd();
}

void gradatePolygon(point2D_t pnt[], int n, color_t color[]) {
	int i;
	glBegin(GL_POLYGON);
	for (i = 0; i < n; i++)
	{
		setColor(color[i]);
		glVertex2f(pnt[i].x, pnt[i].y);
	}
	glEnd();
}

// function that returns a word from 'line' with position 'index'
// note that this is not a zero based index, first word is 1,
// second is 2 etc ..

string strWord(int index, string line) {
	int count = 0; // number of read words
	string word; // the resulting word
	for (int i = 0; i < line.length(); i++) { // iterate over all characters in 'line'
		if (line[i] == ' ') { // if this character is a space we might be done reading a word from 'line
			if (line[i + 1] != ' ') { // next character is not a space, so we are done reading a word
				count++; // increase number of read words
				if (count == index) { // was this the word we were looking for?
					return word; // yes it was, so return it
				}
				word = ""; // nope it wasn't .. so reset word and start over with the next one in 'line'
			}
		}
		else { // not a space .. so append the character to 'word'
			word += line[i];
			if (i + 1 == line.length())
			{
				return word;
			}
		}
	}
}

void readDataset(string filename, object3D_t &fileObj) {
	// File to read
	ifstream file(filename);
	string content;
	string result;

	int maxVertices = 0;
	int maxFaces = 0;

	// Loop until reach End of File
	int index = 1;
	while (getline(file, content))
	{
		// Get number of Vertices and Faces
		if (index == 2)
		{
			result = strWord(1, content);
			fileObj.numOfVertices = stoi(result);
			result = strWord(2, content);
			fileObj.numOfFaces = stoi(result);

			// Set for next looping
			maxVertices = 3 + fileObj.numOfVertices;
			maxFaces = maxVertices + fileObj.numOfFaces;
		}
		else if (index >= 3 && index < maxVertices)
		{
			// titik x y z
			float titik[3];
			for (int i = 1; i <= 3; i++)
			{
				result = strWord(i, content);
				titik[i - 1] = strtof(result.c_str(), 0);
			}

			// save point vertices
			point3D_t pnt = { titik[0], titik[1], titik[2] };
			fileObj.pnt[index - 3] = pnt;

			// save color
			float warna[3];
			for (int i = 4; i <= 6; i++)
			{
				result = strWord(i, content);
				warna[i - 4] = strtof(result.c_str(), 0);
			}
			color_t col = { warna[0], warna[1], warna[2] };
			fileObj.col[index - 3] = col;
		}
		else if (index >= maxVertices && index < maxFaces)
		{
			// num of point | titik pembentuk faces
			float titik2[4];
			for (int i = 1; i <= 4; i++)
			{
				result = strWord(i, content);
				titik2[i - 1] = strtof(result.c_str(), 0);
			}
			// set jumlah tiitk pembuat face
			fileObj.fc[index - 3 - fileObj.numOfVertices].numOfVertices = titik2[0];
			// Set titik pembentuk faces
			fileObj.fc[index - 3 - fileObj.numOfVertices].pnt[0] = titik2[1];
			fileObj.fc[index - 3 - fileObj.numOfVertices].pnt[1] = titik2[2];
			fileObj.fc[index - 3 - fileObj.numOfVertices].pnt[2] = titik2[3];
		}

		index = index + 1;
	}
}

void writeObjectToFile(string filePath, object3D_t object) {
	ofstream myfile(filePath);
	float roundDigit = 100;
	if (myfile.is_open())
	{
		myfile << "COFF\n";
		myfile << object.numOfVertices << " " << object.numOfFaces << " " << 0 << endl;
		for (int i = 0; i < object.numOfVertices; i++)
		{
			myfile << roundf(object.pnt[i].x * roundDigit) / roundDigit << " "
				<< roundf(object.pnt[i].y * roundDigit) / roundDigit << " "
				<< roundf(object.pnt[i].z * roundDigit) / roundDigit << " ";
			myfile << object.col[i].r << " "
				<< object.col[i].g << " "
				<< object.col[i].b << " ";
			myfile << 255 << endl;
		}
		for (int i = 0; i < object.numOfFaces; i++)
		{
			myfile << object.fc[i].numOfVertices;
			for (int j = 0; j < object.fc[i].numOfVertices; j++)
			{
				myfile << " " << object.fc[i].pnt[j];
			}
			if (i != object.numOfFaces - 1)
			{
				myfile << endl;
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file";
}

static void MakeKubus(object3D_t &kerucut, int n, float h, float r) {
	kerucut.numOfVertices = n * 2;
	kerucut.numOfFaces = n + 2;
	kerucut.numOfColors = n * 2;

	// Point
	for (int i = 0; i < kerucut.numOfVertices; i++)
	{
		float rad = i * 2 * M_PI / n;

		// point Bawah
		if (i < n)
		{
			kerucut.pnt[i].x = r * cos(rad);
			kerucut.pnt[i].y = r * sin(rad);
			kerucut.pnt[i].z = 0;
		}

		else
		{
			kerucut.pnt[i].x = r * cos(rad);
			kerucut.pnt[i].y = r * sin(rad);
			kerucut.pnt[i].z = h;
		}

	}

	// Faces
	for (int  i = 0; i < n; i++)
	{
		if (i == n-1)
		{
			kerucut.fc[i] = { 4,{ i , i - n + 1, i + 1, i + n } };
		}
		else
		{
			kerucut.fc[i] = { 4,{ i, i + 1, i + n + 1, i + n } };
		}
	}

	kerucut.fc[kerucut.numOfFaces - 2].numOfVertices = n;
	for (int i = 0; i < n; i++)
	{
		kerucut.fc[kerucut.numOfFaces - 2].pnt[i] = i;
	}

	kerucut.fc[kerucut.numOfFaces - 1].numOfVertices = n;
	for (int i = 0; i < n; i++)
	{
		kerucut.fc[kerucut.numOfFaces - 1].pnt[i] = i+n;
	}
}

static void MakeKerucut(object3D_t &kerucut, int n, float h, float r) {
	kerucut.numOfVertices = n + 2;
	kerucut.numOfColors = n + 2;
	kerucut.numOfFaces = n * 2;

	kerucut.pnt[0].x = 0;
	kerucut.pnt[0].y = 0;
	kerucut.pnt[0].z = h;
	kerucut.col[0].r = 255;
	kerucut.col[0].g = 51;
	kerucut.col[0].b = 153;

	// Point
	for (int i = 0; i < kerucut.numOfVertices; i++)
	{
		float s = i * 2 * M_PI / n;
		kerucut.pnt[i + 1].x = r * cos(s);
		kerucut.pnt[i + 1].y = r * sin(s);
		kerucut.pnt[i + 1].z = 0.;

		kerucut.col[i + 1].r = rand() % 150;
		kerucut.col[i + 1].g = rand() % 150;
		kerucut.col[i + 1].b = rand() % 150;
	}

	kerucut.pnt[kerucut.numOfVertices - 1] = { 0, 0, 0 };

	// Faces
	for (int i = 1; i < n; i++)
	{
		kerucut.fc[i - 1] = { 3,{ 0, i + 1, i } };
	}

	kerucut.fc[n - 1] = { 3,{ 0, 1, n } };

	// Face alas
	for (int i = 1; i <= n; i++)
	{
		kerucut.fc[i + n - 1] = { 3,{ kerucut.numOfVertices - 1, i, i + 1 } };
	}

	kerucut.fc[kerucut.numOfFaces - 1] = { 3,{ kerucut.numOfVertices - 1, n, 1 } };
}

static void MakeTugas(object3D_t &kerucut, int n, float h, float r) {
	kerucut.numOfVertices = n * 4;
	kerucut.numOfColors = n * 4;
	kerucut.numOfFaces = (n*3) + 4;

	// Point
	for (int i = 0; i < kerucut.numOfVertices; i++)
	{
		float rad = i * 2 * M_PI / n;

		// point Alas 1
		if (i < n)
		{
			kerucut.pnt[i].x = r * cos(rad);
			kerucut.pnt[i].y = r * sin(rad);
			kerucut.pnt[i].z = -(h/2);

			//kerucut.col[i].r = rand() % 150;
			//kerucut.col[i].g = rand() % 150;
			//kerucut.col[i].b = rand() % 150;
		}

		else if (i >= n && i < n * 2)
		{
			kerucut.pnt[i].x = (r * 3) * cos(rad);
			kerucut.pnt[i].y = (r * 3) * sin(rad);
			kerucut.pnt[i].z = h/2;

			//kerucut.col[i].r = rand() % 150;
			//kerucut.col[i].g = rand() % 150;
			//kerucut.col[i].b = rand() % 150;
		}
		else if (i >= n*2 && i < n * 3)
		{
			kerucut.pnt[i].x = r * cos(rad);
			kerucut.pnt[i].y = r * sin(rad);
			kerucut.pnt[i].z = h * 2;

//			kerucut.col[i].r = rand() % 150;
//			kerucut.col[i].g = rand() % 150;
//			kerucut.col[i].b = rand() % 150;
		}
		else
		{
			kerucut.pnt[i].x = (r*2) * cos(rad);
			kerucut.pnt[i].y = (r * 2) * sin(rad);
			kerucut.pnt[i].z = h * 3;

//			kerucut.col[i].r = rand() % 150;
//			kerucut.col[i].g = rand() % 150;
//			kerucut.col[i].b = rand() % 150;
		}

	}

	// Faces
	//tutup bawah
	kerucut.fc[0].numOfVertices = n;
	for (int i = 0; i < n; i++) {
		kerucut.fc[0].pnt[i] = i;
	}

	//sisi lantai 1
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n] = { 4,{ n - 1, 0, n, (n - 1) + n } };
			printf("%d %d %d %d \n", n - 1, 0, n, (n - 1) + n);
		}
		else {
			kerucut.fc[i + 1] = { 4,{ i, i + (n - (n - 1)), i + (n + 1), i + n } };
			printf("%d %d %d %d \n", i, i + (n - (n - 1)), i + (n + 1), i + n);
		}
	}

	//sisi lantai 2
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n * 2] = { 4,{ i + n, n, n * 2 , i + (n * 2) } };
			printf("%d %d %d %d \n", i + n, n, n * 2, i + (n * 2));
		}
		else {
			kerucut.fc[i + 1 + n] = { 4,{ i + n, i + (n + 1), (n * 2) + i + 1 , i + (n * 2) } };
			printf("%d %d %d %d \n", i + n, i + (n + 1), (n * 2) + i + 1, i + (n * 2));
		}
	}

	//sisi lantai 3
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n * 3] = { 4,{ i + (n * 2), n * 2, n * 3 , i + (n * 3) } };
			printf("%d %d %d %d \n", i + (n * 2), n * 2, n * 3, i + (n * 3));
		}
		else {
			kerucut.fc[i + (n*2) + 1] = { 4,{ i + (n * 2), i + (n * 2) + 1, (n * 3) + i + 1 , i + (n * 3) } };
			printf("%d %d %d %d \n", i + (n * 2), i + (n * 2) + 1, (n * 3) + i + 1, i + (n * 3));
		}
	}


}

static void MakePagoda(object3D_t &kerucut, int n, float h, float r) {

	kerucut.numOfVertices = 21;
	kerucut.numOfFaces = 23;
	kerucut.numOfColors = 21;

	// Point
	for (int i = 0; i < kerucut.numOfVertices; i++)
	{
		float s = i * 2 * M_PI / n;

		//kotak bawah
		if (i < n) {
			kerucut.pnt[i].x = r * cos(s);
			kerucut.pnt[i].y = r * sin(s);
			kerucut.pnt[i].z = -(h / 2);

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

			//kotak atas 1 dalam
		}
		else if (i >= n && i < n * 2) {
			kerucut.pnt[i].x = r * cos(s);
			kerucut.pnt[i].y = r * sin(s);
			kerucut.pnt[i].z = h / 2;

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

			//kotak atas 1 luar
		}
		else if (i >= n * 2 && i < n * 3) {
			kerucut.pnt[i].x = (r + (r / 2)) * cos(s);
			kerucut.pnt[i].y = (r + (r / 2)) * sin(s);
			kerucut.pnt[i].z = h / 2;

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

			//kotak atas 2 dalam
		}
		else if (i >= n * 3 && i < n * 4) {
			kerucut.pnt[i].x = r * cos(s);
			kerucut.pnt[i].y = r * sin(s);
			kerucut.pnt[i].z = h;

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

			//kotak atas 2 luar
		}
		else if (i >= n * 4 && i < n * 5) {
			kerucut.pnt[i].x = (r + (r / 2)) * cos(s);
			kerucut.pnt[i].y = (r + (r / 2)) * sin(s);
			kerucut.pnt[i].z = h;

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

			//pucuk
		}
		else {
			kerucut.pnt[i] = { 0, 0, 3 * h };
		}
	}

	//tutup bawah
	kerucut.fc[0].numOfVertices = n;
	for (int i = 0; i < n; i++) {
		kerucut.fc[0].pnt[i] = i;
	}

	//sisi lantai 1 dalam
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n] = { 4,{ n - 1, 0, n, (n - 1) + n } };
			printf("%d %d %d %d \n", n - 1, 0, n, (n - 1) + n);
		}
		else {
			kerucut.fc[i + 1] = { 4,{ i, i + (n - (n - 1)), i + (n + 1), i + n } };
			printf("%d %d %d %d \n", i, i + (n - (n - 1)), i + (n + 1), i + n);
		}
	}

	//sisi atas 1 luar
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n * 2] = { 4,{ (2 * n) - 1, n, 2 * n, (n - 1) + 2 * n } };
			printf("%d %d %d %d \n", n - 1, 0, n, (n - 1) + n);
		}
		else {
			kerucut.fc[i + 1 + n] = { 4,{ i + n, i + (n + 1), i + (2 * n) + 1, i + (n * 2) } };
			printf("%d %d %d %d \n", i + n, i + (n + 1), i + (2 * n) + 1, i + (n * 2));
		}
	}

	//sisi lantai 2
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n * 3] = { 4,{ (3 * n) - 1, n * 2, 3 * n, (n - 1) + 3 * n } };
			printf("%d %d %d %d \n", (3 * n) - 1, n * 2, 3 * n, (n - 1) + 3 * n);
		}
		else {
			kerucut.fc[i + 1 + n * 2] = { 4,{ i + (n * 2), i + (2 * n) + 1, i + (3 * n) + 1, i + (n * 3) } };
			printf("%d %d %d %d \n", i + (n * 2), i + (2 * n) + 1, i + (3 * n) + 1, i + (n * 3));
		}
	}

	//sisi tutup 2 dalam
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n * 4] = { 4,{ (4 * n) - 1, n * 3, 4 * n, (n - 1) + 4 * n } };
			printf("%d %d %d %d \n", (3 * n) - 1, n * 2, 3 * n, (n - 1) + 3 * n);
		}
		else {
			kerucut.fc[i + 1 + n * 3] = { 4,{ i + (n * 3), i + (3 * n) + 1, i + (4 * n) + 1, i + (n * 4) } };
			printf("%d %d %d %d \n", i + (n * 3), i + (3 * n) + 1, i + (4 * n) + 1, i + (n * 4));
		}
	}

	//sisi pucuk
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n * 5] = { 3,{ (n * 5) - 1, n * 5, n * 4 } };
			printf("%d %d %d %d \n", (n * 5) - 1, n * 5, n * 4);
		}
		else {
			kerucut.fc[i + 1 + n * 4] = { 3,{ i + (n * 4), n * 5, i + (n * 4) + 1 } };
			printf("%d %d %d %d \n", i + (n * 4), n * 5, i + (n * 4) + 1);
		}
	}
}

static void MakeLove(object3D_t &kerucut, int n, float h, float r) {

	kerucut.numOfVertices = n * 2;
	kerucut.numOfFaces = n + 2;
	kerucut.numOfColors = n * 2;

	kerucut.pnt[0] = { 1,0,0 };
	kerucut.pnt[1] = { 1,0.5,0 };
	kerucut.pnt[2] = { 0.75,0.75,0 };
	kerucut.pnt[3] = { 0.25,0.75,0 };
	kerucut.pnt[4] = { 0,0.5,0 };
	kerucut.pnt[5] = { -0.25,0.75,0 };
	kerucut.pnt[6] = { -0.75,0.75,0 };
	kerucut.pnt[7] = { -1,0.5,0 };
	kerucut.pnt[8] = { -1,0,0 };
	kerucut.pnt[9] = { 0,-1,0 };

	kerucut.pnt[10] = { 1,0,h };
	kerucut.pnt[11] = { 1,0.5,h };
	kerucut.pnt[12] = { 0.75,0.75,h };
	kerucut.pnt[13] = { 0.25,0.75,h };
	kerucut.pnt[14] = { 0,0.5,h };
	kerucut.pnt[15] = { -0.25,0.75,h };
	kerucut.pnt[16] = { -0.75,0.75,h };
	kerucut.pnt[17] = { -1,0.5,h };
	kerucut.pnt[18] = { -1,0,h };
	kerucut.pnt[19] = { 0,-1,h };

	kerucut.fc[0] = { 4,{ 0,1,11,10 } };
	kerucut.fc[1] = { 4,{ 1,2,12,11 } };
	kerucut.fc[2] = { 4,{ 2,3,13,12 } };
	kerucut.fc[3] = { 4,{ 3,4,14,13 } };
	kerucut.fc[4] = { 4,{ 4,5,15,14 } };
	kerucut.fc[5] = { 4,{ 5,6,16,15 } };
	kerucut.fc[6] = { 4,{ 6,7,17,16 } };
	kerucut.fc[7] = { 4,{ 7,8,18,17 } };
	kerucut.fc[8] = { 4,{ 8,9,19,18 } };
	kerucut.fc[9] = { 4,{ 9,0,10,19 } };

	kerucut.fc[10] = { 11,{ 0,1,2,3,4,5,6,7,8,9,0 } };
	kerucut.fc[11] = { 11,{ 10,11,12,13,14,15,16,17,18,19,10 } };

}

static void MakePensil(object3D_t &kerucut, int n, float h, float r) {

	kerucut.numOfVertices = (n * 2) + 1;
	kerucut.numOfFaces = (n * 2) + 1;
	kerucut.numOfColors = (n * 2) + 1;

	// Point
	for (int i = 0; i < kerucut.numOfVertices; i++)
	{
		float s = i * 2 * M_PI / n;

		//titik depan
		if (i < n) {
			kerucut.pnt[i].x = r * cos(s);
			kerucut.pnt[i].y = r * sin(s);
			kerucut.pnt[i].z = -(h / 2);

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;
			//titik belakang
		}
		else if (i == kerucut.numOfVertices - 1) {
			kerucut.pnt[i].x = 0;
			kerucut.pnt[i].y = 0;
			kerucut.pnt[i].z = (h + 0.25);

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;
		}
		else {
			kerucut.pnt[i].x = r * cos(s);
			kerucut.pnt[i].y = r * sin(s);
			kerucut.pnt[i].z = (h / 2);

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;
		}
	}

	// buat Faces sisi
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n - 1] = { 4,{ n - 1, n - n, n - (n - n), (n - 1) + n } };
			printf("%d %d %d %d \n", n - 1, n - n, n - (n - n), (n - 1) + n);
		}
		else {
			kerucut.fc[i] = { 4,{ i + (n - n), i + (n - (n - 1)), i + (n + 1), i + n } };
			printf("%d %d %d %d \n", i + (n - n), i + (n - (n - 1)), i + (n + 1), i + n);
		}
	}

	kerucut.fc[n].numOfVertices = n;
	for (int i = 0; i < n; i++) {
		kerucut.fc[n].pnt[i] = i;
	}

	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[(n * 2)] = { 3,{ (n * 2) - 1, n * 2, n } };
			printf("%d %d %d %d \n", (n * 2) + 1, (n * 2) - 1, n * 2, n);
		}
		else {
			kerucut.fc[i + n + 1] = { 3,{ i + n, n * 2, i + n + 1 } };
			printf("%d %d %d %d \n", i + (n + 1), i + n, n * 2, i + n + 1);
		}
	}

}

static void MakeSedotan(object3D_t &kerucut, int n, float h, float r) {
	kerucut.numOfVertices = n * 4;
	kerucut.numOfFaces = n * 4;
	kerucut.numOfColors = n * 4;

	// gawe point 
	for (int i = 0; i < kerucut.numOfVertices; i++)
	{
		float theta = i * 2 * M_PI / n;

		if (i < n * 2)
		{
			if (i < n)
			{
				kerucut.pnt[i].x = r * cos(theta);
				kerucut.pnt[i].y = r * sin(theta);
				kerucut.pnt[i].z = 0;

				kerucut.col[i].r = rand() % 150;
				kerucut.col[i].g = rand() % 150;
				kerucut.col[i].b = rand() % 150;
			}
			else
			{
				kerucut.pnt[i].x = (r * 2) * cos(theta);
				kerucut.pnt[i].y = (r*2) * sin(theta);
				kerucut.pnt[i].z = 0;

				kerucut.col[i].r = rand() % 150;
				kerucut.col[i].g = rand() % 150;
				kerucut.col[i].b = rand() % 150;
			}
		}
		else
		{
			if (i < n*3)
			{
				kerucut.pnt[i].x = r * cos(theta);
				kerucut.pnt[i].y = r * sin(theta);
				kerucut.pnt[i].z = h;

				kerucut.col[i].r = rand() % 150;
				kerucut.col[i].g = rand() % 150;
				kerucut.col[i].b = rand() % 150;
			}
			else
			{
				kerucut.pnt[i].x = (r*2) * cos(theta);
				kerucut.pnt[i].y = (r * 2) * sin(theta);
				kerucut.pnt[i].z = h;

				kerucut.col[i].r = rand() % 150;
				kerucut.col[i].g = rand() % 150;
				kerucut.col[i].b = rand() % 150;
			}
		}
	}

	for (int i = 0; i < n; i++)
	{
		if (i == n - 1)
		{
			kerucut.fc[i] = { 4,{ i , i - n + 1, i + n + 1, i + (n * 2) } };
		}
		else
		{
			kerucut.fc[i] = { 4,{ i, i + 1, i + (n * 2) + 1, i + (n*2) } };
		}
	}

	for (int i = n; i < n * 2; i++)
	{
		if (i == (n*2) - 1)
		{
			kerucut.fc[i] = { 4,{ i, i - n + 1, i + n + 1, i + (n * 2) } };
		}
		else
		{
			kerucut.fc[i] = { 4,{ i, i +1, i + (n*2) + 1, i + (n*2) } };
		}
	}

	for (int i = n*2; i < n * 3; i++)
	{
		if (i == (n * 3) - 1)
		{
			kerucut.fc[i] = { 4,{ i - (n * 2), i - (n * 3) + 1, i - (n*2) + 1, i - n } };
		}
		else
		{
			kerucut.fc[i] = { 4,{ i - (n*2), i - (n *2) + 1, i - n + 1, i - n  } };
		}
	}

	for (int i = n * 3; i < n * 4; i++)
	{
		if (i == (n * 4) - 1)
		{
			kerucut.fc[i] = { 4,{ i - n, i - (n * 2) + 1, i - n + 1, i  } };
		}
		else
		{
			kerucut.fc[i] = { 4,{ i - n, i - n + 1, i + 1, i } };
		}
	}
}

static void MakeDonut(object3D_t &kerucut, int n, float h, float r) {

	kerucut.numOfVertices = n * n;
	kerucut.numOfFaces = n * 8;
	kerucut.numOfColors = n * n;

	// Point
	for (int i = 0; i < kerucut.numOfVertices; i++)
	{
		float s = i * 2 * M_PI / n;

		if (i < n) {
			//0-7
			kerucut.pnt[i].x = r * cos(s);
			kerucut.pnt[i].y = r * sin(s);
			kerucut.pnt[i].z = 0;

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

		}
		else if (i >= n && i < n * 2) {
			//8-15
			kerucut.pnt[i].x = (r * 2) * cos(s);
			kerucut.pnt[i].y = (r * 2) * sin(s);
			kerucut.pnt[i].z = -(h / 4);

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

		}
		else if (i >= n * 2 && i < n * 3) {
			//16-23
			kerucut.pnt[i].x = (r * 3) * cos(s);
			kerucut.pnt[i].y = (r * 3) * sin(s);
			kerucut.pnt[i].z = -(h / 4);

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

		}
		else if (i >= n * 3 && i < n * 4) {
			//24-31
			kerucut.pnt[i].x = (r * 4) * cos(s);
			kerucut.pnt[i].y = (r * 4) * sin(s);
			kerucut.pnt[i].z = 0;

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;
		}
		else if (i >= n * 4 && i < n * 5) {
			//32-39
			kerucut.pnt[i].x = (r * 4) * cos(s);
			kerucut.pnt[i].y = (r * 4) * sin(s);
			kerucut.pnt[i].z = h;

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

		}
		else if (i >= n * 5 && i < n * 6) {
			//40-47
			kerucut.pnt[i].x = (r * 3) * cos(s);
			kerucut.pnt[i].y = (r * 3) * sin(s);
			kerucut.pnt[i].z = h + (h / 4);

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

		}
		else if (i >= n * 6 && i < n * 7) {
			kerucut.pnt[i].x = (r * 2) * cos(s);
			kerucut.pnt[i].y = (r * 2) * sin(s);
			kerucut.pnt[i].z = h + (h / 4);

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;

		}
		else {
			kerucut.pnt[i].x = r * cos(s);
			kerucut.pnt[i].y = r * sin(s);
			kerucut.pnt[i].z = h;

			kerucut.col[i].r = rand() % 150;
			kerucut.col[i].g = rand() % 150;
			kerucut.col[i].b = rand() % 150;
		}
	}

	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[i] = { 4,{ i, i - (n - 1), i + 1, i + n } };
		}
		else {
			kerucut.fc[i] = { 4,{ i, i + 1, i + (n + 1), i + n } };
		}
	}

	for (int i = n; i < n * 2; i++) {
		if (i == ((n * 2) - 1)) {
			kerucut.fc[i] = { 4,{ i, i - (n - 1), i + 1, i + n } };
		}
		else {
			kerucut.fc[i] = { 4,{ i, i + 1, i + (n + 1), i + n } };
		}
	}

	for (int i = n * 2; i < n * 3; i++) {
		if (i == ((n * 3) - 1)) {
			kerucut.fc[i] = { 4,{ i, i - (n - 1), i + 1, i + n } };
		}
		else {
			kerucut.fc[i] = { 4,{ i, i + 1, i + (n + 1), i + n } };
		}
	}

	for (int i = n * 3; i < n * 4; i++) {
		if (i == ((n * 4) - 1)) {
			kerucut.fc[i] = { 4,{ i, i - (n - 1), i + 1, i + n } };
		}
		else {
			kerucut.fc[i] = { 4,{ i, i + 1, i + (n + 1), i + n } };
		}
	}

	for (int i = n * 4; i < n * 5; i++) {
		if (i == ((n * 5) - 1)) {
			kerucut.fc[i] = { 4,{ i, i - (n - 1), i + 1, i + n } };
		}
		else {
			kerucut.fc[i] = { 4,{ i, i + 1, i + (n + 1), i + n } };
		}
	}

	for (int i = n * 5; i < n * 6; i++) {
		if (i == ((n * 6) - 1)) {
			kerucut.fc[i] = { 4,{ i, i - (n - 1), i + 1, i + n } };
		}
		else {
			kerucut.fc[i] = { 4,{ i, i + 1, i + (n + 1), i + n } };
		}
	}

	for (int i = n * 6; i < n * 7; i++) {
		if (i == ((n * 7) - 1)) {
			kerucut.fc[i] = { 4,{ i, i - (n - 1), i + 1, i + n } };
		}
		else {
			kerucut.fc[i] = { 4,{ i, i + 1, i + (n + 1), i + n } };
		}
	}

	//56 - 63
	for (int i = n * 7; i < n * 8; i++) {
		if (i == ((n * 8) - 1)) {
			kerucut.fc[i] = { 4,{ i - (n * 7), i - ((n * 8) - 1), i + 1 - n, i } };
		}
		else {
			kerucut.fc[i] = { 4,{ i - (n * 7), i + 1 - (n * 7), i + (n + 1) - n, i } };
		}
	}

}

static void MakePipa(object3D_t &kerucut, int n, float h, float r) {

	kerucut.numOfVertices = (n * 2) * 2;
	kerucut.numOfFaces = n * 4;
	kerucut.numOfColors = n * n;

	// Point
	for (int i = 0; i <= kerucut.numOfVertices; i++)
	{
		float s = i * 2 * M_PI / n;
		//titik depan
		if (i < kerucut.numOfVertices / 2) {
			//tabung depan dalam
			if (i < n) {
				kerucut.pnt[i].x = r * cos(s);
				kerucut.pnt[i].y = r * sin(s);
				kerucut.pnt[i].z = -(h / 2);

				kerucut.col[i].r = rand() % 150;
				kerucut.col[i].g = rand() % 150;
				kerucut.col[i].b = rand() % 150;
			}
			//tabung depan luar
			else {
				kerucut.pnt[i].x = (r + (r / 2)) * cos(s);
				kerucut.pnt[i].y = (r + (r / 2)) * sin(s);
				kerucut.pnt[i].z = -(h / 2);

				kerucut.col[i].r = rand() % 150;
				kerucut.col[i].g = rand() % 150;
				kerucut.col[i].b = rand() % 150;
			}


			//titik belakang
		}
		else {
			//tabung belakang dalam
			if (i < (kerucut.numOfVertices / 2) + n) {
				kerucut.pnt[i].x = r * cos(s);
				kerucut.pnt[i].y = r * sin(s);
				kerucut.pnt[i].z = (h / 2);

				kerucut.col[i].r = rand() % 150;
				kerucut.col[i].g = rand() % 150;
				kerucut.col[i].b = rand() % 150;
			}
			//tabung belakang luar
			else {
				kerucut.pnt[i].x = (r + (r / 2)) * cos(s);
				kerucut.pnt[i].y = (r + (r / 2)) * sin(s);
				kerucut.pnt[i].z = (h / 2);

				kerucut.col[i].r = rand() % 150;
				kerucut.col[i].g = rand() % 150;
				kerucut.col[i].b = rand() % 150;
			}
		}
	}

	// buat Face tutup bawah 0-n
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[n - 1] = { 4,{ ((kerucut.numOfVertices / 2) + n) - 1, ((kerucut.numOfVertices / 2) + n) - n, (kerucut.numOfVertices / 2) + n, (((kerucut.numOfVertices / 2) + n) - 1) + n } };
			printf("%d %d %d %d \n", n - 1, n - n, n - (n - n), (n - 1) + n);
		}
		else {
			kerucut.fc[i] = { 4,{ (i + (kerucut.numOfVertices / 2)), (i + (kerucut.numOfVertices / 2)) + (n - (n - 1)), (i + (kerucut.numOfVertices / 2)) + (n + 1), (i + (kerucut.numOfVertices / 2)) + n } };
			printf("%d %d %d %d \n", i + (n - n), i + (n - (n - 1)), i + (n + 1), i + n);
		}
	}

	//buat face badan dalam n+n
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {

			kerucut.fc[(n * 2) - 1] = { 4,{ n - 1, n - n, (n * 2), ((n * 2) - 1) + n } };
			printf("%d %d %d %d \n", n - 1, n - n, (n * 2), ((n * 2) - 1) + n);
		}
		else {

			kerucut.fc[i + n] = { 4,{ i, i + (n - (n - 1)), i + ((n * 2) + 1), i + (n * 2) } };
			printf("%d %d %d %d \n", i, i + (n - (n - 1)), i + ((n * 2) + 1), i + (n * 2));
		}
	}

	//buat face badan luar 0
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[((n * 3) + n) - 1] = { 4,{ (n * 2) - 1, n, (n * 3), ((n * 3) - 1) + n } };
			printf("%d %d %d %d \n", (n * 2) - 1, n, (n * 3), ((n * 3) - 1) + n);
		}
		else {
			kerucut.fc[i + (n * 3)] = { 4,{ i + n, (i + n) + 1, i + ((n * 3) + 1), i + (n * 3) } };
			printf("%d %d %d %d \n", i + n, (i + n) + 1, i + ((n * 3) + 1), i + (n * 3));
		}
	}

	// buat Faces tutup atas
	for (int i = 0; i < n; i++) {
		if (i == (n - 1)) {
			kerucut.fc[((n * 2) + n) - 1] = { 4,{ n - 1, n - n, n - (n - n), (n - 1) + n } };
			printf("%d %d %d %d \n", n - 1, n - n, n - (n - n), (n - 1) + n);
		}
		else {
			kerucut.fc[i + (n * 2)] = { 4,{ i + (n - n), i + (n - (n - 1)), i + (n + 1), i + n } };
			printf("%d %d %d %d \n", i + (n - n), i + (n - (n - 1)), i + (n + 1), i + n);
		}
	}

}

void createGrid() {
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(-200.0, 0.0, 0.0);
	glVertex3f(200.0, 0.0, 0.0);
	glVertex3f(0.0, -200.0, 0.0);
	glVertex3f(0.0, 200.0, 0.0);
	glEnd();
}

void create3DObject(object3D_t object, color_t col) {
	for (int i = 0; i<object.numOfFaces; i++) {
		setColor(col);
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j<object.fc[i].numOfVertices; j++) {
			int p = object.fc[i].pnt[j];
			float x = object.pnt[p].x;
			float y = object.pnt[p].y;
			glVertex3f(x, y, 0.0);
		}
		glEnd();
	}
}

void drawMyObject() {
	object3D_t obj;
	readDataset("fileku.txt", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			//drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			//drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();
}

void drawKerucut() {
	object3D_t obj;
	MakeKerucut(obj, 4, 1, 0.5);

	// Save to File
	writeObjectToFile("kerucut.off", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			//drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			//drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();
}

void drawSedotan() {
	object3D_t obj;
	MakeSedotan(obj, 8, 2, 0.225);

	// Save to File
	writeObjectToFile("sedotan.txt", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			//drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();
}

void drawKubusManual() {
	object3D_t obj;

	obj.numOfVertices = 8;
	obj.numOfFaces = 6;
	obj.numOfColors = 8;

	for (int i = 0; i < obj.numOfVertices; i++)
	{
		float rad = i * 2 * M_PI / 4;

		if (i < 4)
		{
			obj.pnt[i].x = 0.5 * cos(rad);
			obj.pnt[i].y = 0.5 * sin(rad);
			obj.pnt[i].z = 0;
		}

		else
		{
			obj.pnt[i].x = 0.5 * cos(rad);
			obj.pnt[i].y = 0.5 * sin(rad);
			obj.pnt[i].z = 1;
		}
	}

	// Save to File
	writeObjectToFile("kubuskantal.txt", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}


	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();
}

void drawKubus() {
	object3D_t obj;
	MakeKubus(obj, 4, 0.5, 0.5);

	// Save to File
	writeObjectToFile("kubus.txt", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}


	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();
}

void drawPagoda() {
	object3D_t obj;
	MakePagoda(obj, 4, 0.5, 0.5);

	// Save to File
	writeObjectToFile("pagoda.off", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}


	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();

}

void drawPensil() {
	object3D_t obj;
	MakePensil(obj, 8, 1, 0.5);

	// Save to File
	writeObjectToFile("pensil.off", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}


	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();

}

void drawLove() {
	object3D_t obj;
	MakeLove(obj, 10, 0.5, 0.5);

	// Save to File
	writeObjectToFile("love.off", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}


	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();

}

void drawDonut() {
	object3D_t obj;
	MakeDonut(obj, 20, 0.5, 0.225);

	// Save to File
	writeObjectToFile("donut.off", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}


	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();

}

void drawPipa() {
	object3D_t obj;
	MakePipa(obj, 20, 1, 0.225);

	// Save to File
	writeObjectToFile("pipa.off", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();

}

void drawTugas() {
	object3D_t obj;
	MakeTugas(obj, 20, 0.5, 0.2);

	// Save to File
	writeObjectToFile("tugas.off", obj);

	vector3D_t normalVector, vec[1000], vecbuff[1000];
	point2D_t titik2d[1000];
	float normalzi = 0.;
	color_t warna[1000];

	// Draw Grid
	createGrid();
	matrix3D_t tilting = rotationX(sudut) * rotationY(sudut);

	for (int i = 0; i<obj.numOfVertices; i++)
	{
		vec[i] = point2vector(obj.pnt[i]);
		vec[i] = tilting * vec[i];
	}

	// Invisible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi < 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//drawPolygon(titik2d, obj.fc[i].numOfVertices, { 0, 0, 255 });
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	// Visible
	for (int i = 0; i<obj.numOfFaces; i++)
	{
		for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
			vecbuff[j] = vec[obj.fc[i].pnt[j]];
		}
		normalVector = (vecbuff[1] - vecbuff[0]) ^ (vecbuff[2] - vecbuff[0]);
		normalzi = normalVector.v[2];
		if (normalzi > 0.)
		{
			for (int j = 0; j < obj.fc[i].numOfVertices; j++) {
				titik2d[j] = vector2point2d(vec[obj.fc[i].pnt[j]]);
				warna[j] = obj.col[obj.fc[i].pnt[j]];
			}
			//gradatePolygon(titik2d, obj.fc[i].numOfVertices, warna);
			//fillPolygon(titik2d, obj.fc[i].numOfVertices, { 255, 0, 0 });
			drawPolyline(titik2d, obj.fc[i].numOfVertices);
		}
	}

	sudut++;
	if (sudut >= 360.0)
		sudut = 0.0;
	glFlush();
}

void userdraw() {
	//drawPipa();
	//drawPensil();
	//drawKerucut();
	//drawMyObject();
	//drawPagoda();
	//drawDonut();
	//drawLove();
	//drawKubus();
	//drawKubusManual();
	//drawSedotan();
	drawTugas();
}

void display(void) { //clear screen
	glClear(GL_COLOR_BUFFER_BIT);
	userdraw();
	glutSwapBuffers();
}

void timer(int value) {
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

void initialize() {
	glClearColor(1, 1, 1, 0);
	glLoadIdentity();
	//gluOrtho2D(-300., 300., -300., 300.);
	glOrtho(-1, 1, -1, 1, 1, -1);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	//Inisialisasi Toolkit
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("My Awesome Drawing");
	initialize();
	glutDisplayFunc(display);
	glutTimerFunc(1, timer, 0);
	glutMainLoop();
	return 0;
}