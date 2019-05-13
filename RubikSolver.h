#ifndef _RUBIK_SOLVER_H_
#define _RUBIK_SOLVER_H_

#include <memory>
#include <iostream>
#include <queue>
using namespace std;

#define URF 0u
#define UFL 1u
#define ULB 2u
#define UBR 3u
#define DFR 4u
#define DLF 5u
#define DBL 6u
#define DRB 7u

#define UR 0u
#define UF 1u
#define UL 2u
#define UB 3u
#define DR 4u
#define DF 5u
#define DL 6u
#define DB 7u
#define FR 8u
#define FL 9u
#define BL 10u
#define BR 11u

#define ROTATION_Y_UP 0
#define ROTATION_X_RIGHT 1
#define ROTATION_Z_FRONT 2 
#define ROTATION_Y_DOWN 3
#define ROTATION_X_LEFT 4
#define ROTATION_Z_BACK 5 

typedef unsigned int CORNER_INDEX;
typedef unsigned int EDGE_INDEX;
typedef unsigned int UINT;

struct corner_rotation_info_t
{
	CORNER_INDEX replace_corner;
	int direction_delta;
};

struct edge_rotation_info_t
{
	EDGE_INDEX replace_edge;
	int direction_delta;
};


static const int factorial_4[] = { 1, 4, 12, 24 };
static const int factorial_8[] = { 1, 8, 56, 336, 1680, 6720, 20160, 40320 };
static const int factorial_12[] = { 1, 12, 132, 1320, 11880, 95040, 665280, 3991680, 19958400, 79833600, 239500800, 479001600 };
static const int factorial[13] = { 1, 1,2,6,24,120,720 ,5040, 40320, 362880, 3628800, 39916800, 479001600 };

static const int phase_1_corner_max_state = 6561;
static const int phase_1_edge_max_state = 4096;
static const int phase_1_UDSlice_max_state = 495;
static const int phase_2_corner_position_max_state = factorial[8];
static const int phase_2_edge_position_max_state = factorial[8];
static const int phase_2_udslice_position_max_state = factorial[4];


static int phase_1_corner_direction_min_dist[phase_1_corner_max_state];
static int phase_1_edge_direction_min_dist[phase_1_edge_max_state];
static int phase_1_udslice_min_dist[phase_1_UDSlice_max_state];

static int phase_2_corner_position_min_dist[40320];
static int phase_2_UDEdge_position_min_dist[40320];
static int  phase_2_UDSlice_position_min_dist[24];

template<int N>
UINT EncodePerm(int * p, const int * factorial)
{
	int pos[N], elem[N];

	for (int i = 0; i != N; ++i)
		pos[i] = elem[i] = i;

	int v = 0, t;
	for (int i = 0; i != N; ++i)
	{
		t = pos[p[i]];
		v += factorial[i] * t;
		pos[elem[N - i - 1]] = t;
		elem[t] = elem[N - i - 1];
	}

	return v;
};

template<int radix, int length>
UINT EncodeSeries(int * series)
{
	UINT res = 0;
	int weight = 1;
	for (int i = 0; i < length; i++)
	{
		res += (series[i] * weight);
		weight *= radix;
	}
	return res;
}

static corner_rotation_info_t corner_rotation_infos[6][8] = {
	{ { UBR , 0 },{ URF , 0 } ,{ UFL , 0 } ,{ ULB , 0 } ,{ DFR , 0 } ,{ DLF , 0 } ,{ DBL , 0 } ,{ DRB , 0 } }	, //up
	{ { DFR , 2 },{ UFL , 0 } ,{ ULB , 0 } ,{ URF , 1 } ,{ DRB , 1 } ,{ DLF , 0 } ,{ DBL , 0 } ,{ UBR , 2 } }	, //right
	{ { UFL , 1 },{ DLF , 2 } ,{ ULB , 0 } ,{ UBR , 0 } ,{ URF , 2 } ,{ DFR , 1 } ,{ DBL , 0 } ,{ DRB , 0 } } , // front 
	{ { URF , 0 },{ UFL , 0 } ,{ ULB , 0 } ,{ UBR , 0 } ,{ DLF , 0 } ,{ DBL , 0 } ,{ DRB , 0 } ,{ DFR , 0 } } , // down
	{ { URF , 0 },{ ULB , 1 } ,{ DBL , 2 } ,{ UBR , 0 } ,{ DFR , 0 } ,{ UFL , 2 } ,{ DLF , 1 } ,{ DRB , 0 } } , // left
	{ { URF , 0 },{ UFL , 0 } ,{ UBR , 1 } ,{ DRB , 2 } ,{ DFR , 0 } ,{ DLF , 0 } ,{ ULB , 2 } ,{ DBL , 1 } }   // back
};

static edge_rotation_info_t edge_rotation_infos[6][12] = {
	{ { UB , 0 } ,{ UR , 0 } ,{ UF , 0 } ,{ UL , 0 } ,{ DR , 0 } ,{ DF , 0 } ,{ DL , 0 } ,{ DB , 0 } ,{ FR , 0 } ,{ FL , 0 } ,{ BL , 0 } ,{ BR , 0 } } , // up
	{ { FR , 0 } ,{ UF , 0 } ,{ UL , 0 } ,{ UB , 0 } ,{ BR , 0 } ,{ DF , 0 } ,{ DL , 0 } ,{ DB , 0 } ,{ DR , 0 } ,{ FL , 0 } ,{ BL , 0 } ,{ UR , 0 } } , // right
	{ { UR , 0 } ,{ FL , 1 } ,{ UL , 0 } ,{ UB , 0 } ,{ DR , 0 } ,{ FR , 1 } ,{ DL , 0 } ,{ DB , 0 } ,{ UF , 1 } ,{ DF , 1 } ,{ BL , 0 } ,{ BR , 0 } } , // front
	{ { UR , 0 } ,{ UF , 0 } ,{ UL , 0 } ,{ UB , 0 } ,{ DF , 0 } ,{ DL , 0 } ,{ DB , 0 } ,{ DR , 0 } ,{ FR , 0 } ,{ FL , 0 } ,{ BL , 0 } ,{ BR , 0 } } , // down
	{ { UR , 0 } ,{ UF , 0 } ,{ BL , 0 } ,{ UB , 0 } ,{ DR , 0 } ,{ DF , 0 } ,{ FL , 0 } ,{ DB , 0 } ,{ FR , 0 } ,{ UL , 0 } ,{ DL , 0 } ,{ BR , 0 } } , // left
	{ { UR , 0 } ,{ UF , 0 } ,{ UL , 0 } ,{ BR , 1 } ,{ DR , 0 } ,{ DF , 0 } ,{ DL , 0 } ,{ BL , 1 } ,{ FR , 0 } ,{ FL , 0 } ,{ UB , 1 } ,{ DB , 1 } }  // back
};

struct RubikSolver
{
public:
	RubikSolver();
	int corner_position[8];
	int corner_direction[8];
	int edge_position[12];
	int edge_direction[12];
	int path_length = 0;

	void									Rotate(int rotation_face, int rotation_count)
	{
		rotation_count = rotation_count % 4;
		corner_rotation_info_t * cur_corner_info = &corner_rotation_infos[rotation_face][0];
		edge_rotation_info_t * cur_edge_info = &edge_rotation_infos[rotation_face][0];

		for (int j = 0; j < rotation_count; j++)
		{
			RubikSolver tmp_cube = *this;

			for (int i = 0; i < 8; i++)
			{
				int replace_corner = cur_corner_info[i].replace_corner;
				corner_position[i] = tmp_cube.corner_position[replace_corner];
				corner_direction[i] = (tmp_cube.corner_direction[replace_corner] + cur_corner_info[i].direction_delta) % 3;
			}

			for (int i = 0; i < 12; i++)
			{
				int replace_edge = cur_edge_info[i].replace_edge;
				edge_position[i] = tmp_cube.edge_position[replace_edge];
				edge_direction[i] = (tmp_cube.edge_direction[replace_edge] + cur_edge_info[i].direction_delta) % 2;

			}

		}
	}
	UINT								EncodeCornerPosition()
	{
		return EncodePerm<8>(corner_position, factorial_8);
	}
	UINT								EncodeCornerDirection()
	{
		return EncodeSeries<3, 8>(corner_direction);
	}
	UINT								EncodeEdgePosition()
	{
		return EncodePerm<12>(edge_position, factorial_12);
	}
	UINT								EncodeEdgeDirection()
	{
		return EncodeSeries<2, 12>(edge_direction);
	}
	UINT								EncodeUDSlice()
	{
		bool tmp[12];
		memset(tmp, false, sizeof(tmp));

		for (UINT i = UR; i <= BR; i++)
		{
			if (edge_position[i] >= FR) tmp[i] = true;
		}
		int k = 3, i = 11;
		UINT res = 0;

		auto Cnk = [](int n, int k)->int {
			return factorial[n] / (factorial[k] * factorial[n - k]);
		};

		while (k >= 0)
		{
			if (tmp[i]) k--;
			else res += Cnk(i, k);
			i--;
		}

		return res;
	}
	UINT								EncodeUDSlicePosition()
	{
		int tmp[4] = { edge_position[FR] - 8 ,edge_position[FL] - 8 ,edge_position[BL] - 8 ,edge_position[BR] - 8 };
		return EncodePerm<4>(tmp, factorial_4);
	}
	UINT								EncodeUDEdgePosition()
	{
		return EncodePerm<8>(edge_position, factorial_8);
	}
};

void										 InitMinDistance(int * buffer, int code_max, UINT(RubikSolver::*encode_func)(), RubikSolver cube, const bool enable_rotation[6][3]);
int										 Phase1Estimate(RubikSolver & cube);
bool										 IDASearch(int depth, int g, int(*estimate_function)(RubikSolver & cube), 
															   RubikSolver cube, const bool enable_rotation[6][3], vector<pair<int, int >> &  path, int prev_face, int & path_length);
void										 Phase1Search(RubikSolver & cube, vector<pair<int, int>> & path);
int										 Phase2Estimate(RubikSolver & cube);
void										 Phase2Search(RubikSolver & cube, vector<pair<int, int >> &  path);

struct MoveEntity
{
	RubikSolver cube;
	int step;
};

const bool phase1_enable_rotation[6][3] = {
	{ true , true , true },{ true , true , true } ,{ true , true , true },{ true , true , true },{ true , true , true },{ true , true , true }
};
const bool phase2_enable_rotation[6][3] = {
	{ true , true , true },{ false , true , false } ,{ false , true , false },{ true , true , true },{ false , true , false } ,{ false , true , false }
};
#endif