#include "RubikSolver.h"

void InitMinDistance(int * buffer, int code_max, UINT(RubikSolver::* encode_func)(), RubikSolver cube, const bool enable_rotation[6][3])
{
	memset(buffer, -1, code_max * sizeof(int));
	UINT start_state = (cube.*encode_func)();
	buffer[start_state] = 0;
	queue<MoveEntity> entity_queue;
	entity_queue.push({ cube , 0 });

	while (!entity_queue.empty())
	{
		MoveEntity top_entity = entity_queue.front();
		entity_queue.pop();

		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (enable_rotation[i][j] == false) continue;
				RubikSolver tmp = top_entity.cube;
				tmp.Rotate(i, j + 1);
				UINT new_state = (tmp.*encode_func)();
				if (buffer[new_state] == -1)
				{
					buffer[new_state] = top_entity.step + 1;
					entity_queue.push({ tmp , top_entity.step + 1 });
				}
			}
		}

	}
}

int Phase1Estimate(RubikSolver & cube)
{
	return max(max(phase_1_corner_direction_min_dist[cube.EncodeCornerDirection()],
		phase_1_edge_direction_min_dist[cube.EncodeEdgeDirection()]),
		phase_1_udslice_min_dist[cube.EncodeUDSlice()]
	);
}

bool IDASearch(int depth, int g, int(*estimate_function)(RubikSolver &cube), RubikSolver cube, const bool enable_rotation[6][3], 
						  vector<pair<int, int>>& path, int prev_face , int & path_length )
{
	int estimate_val = estimate_function(cube);
	if (g + estimate_val > depth) return false;
	if (estimate_val == 0) return true;

	for (int i = 5; i >= 0; i--)
	{
		if (i == prev_face) continue;
		for (int j = 0; j < 3; j++)
		{
			if (enable_rotation[i][j] == false) continue;
			RubikSolver next_cube = cube;
			next_cube.Rotate(i, j + 1);
			path[path_length].first = i;
			path[path_length].second = j + 1;
			path_length++;
			if (IDASearch(depth, g + 1, estimate_function, next_cube, enable_rotation, path, i , path_length) == true)
			{
				return true;
			}
			path_length--;
		}
	}

	return false;
}

void Phase1Search(RubikSolver & cube, vector<pair<int, int>>& path)
{
	for (int depth = 0; ; depth++)
	{
		int path_length = 0;
		if (IDASearch(depth, 0, Phase1Estimate, cube, phase1_enable_rotation, path, -1 , path_length ))
		{
			return;
		}
	}
}

int Phase2Estimate(RubikSolver & cube)
{
	return max(max(phase_2_corner_position_min_dist[cube.EncodeCornerPosition()],
		phase_2_UDEdge_position_min_dist[cube.EncodeUDEdgePosition()]),
		phase_2_UDSlice_position_min_dist[cube.EncodeUDSlicePosition()]
	);
}

void Phase2Search(RubikSolver & cube, vector<pair<int, int>>& path)
{
	for (int depth = 0; ; depth++)
	{
		int path_length = 0;
		if (IDASearch(depth, 0, Phase2Estimate, cube, phase2_enable_rotation, path, -1 , path_length ))
		{
			return;
		}
	}
}

RubikSolver::RubikSolver()
{
	for (int i = 0; i < 8; i++)
	{
		corner_direction[i] = 0;
		corner_position[i] = i;
	}

	for (int i = 0; i < 12; i++)
	{
		edge_position[i] = i;
		edge_direction[i] = 0;
	}
	InitMinDistance(phase_1_edge_direction_min_dist, phase_1_edge_max_state, &RubikSolver::EncodeEdgeDirection, *this, phase1_enable_rotation);
	InitMinDistance(phase_1_corner_direction_min_dist, phase_1_corner_max_state, &RubikSolver::EncodeCornerDirection, *this, phase1_enable_rotation);
	InitMinDistance(phase_1_udslice_min_dist, phase_1_UDSlice_max_state, &RubikSolver::EncodeUDSlice, *this, phase1_enable_rotation);
	InitMinDistance(phase_2_corner_position_min_dist, 40320, &RubikSolver::EncodeCornerPosition, *this, phase2_enable_rotation);
	InitMinDistance(phase_2_UDEdge_position_min_dist, 40320, &RubikSolver::EncodeUDEdgePosition, *this, phase2_enable_rotation);
	InitMinDistance(phase_2_UDSlice_position_min_dist, 24, &RubikSolver::EncodeUDSlicePosition, *this, phase2_enable_rotation);

}
