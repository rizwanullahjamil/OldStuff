#include <stdio.h>
#include <string.h>

typedef int Grid[9 * 9];
struct Unit
{
  int cells[9];
  struct Unit *intersections[12];
  int num_intersections;
};

static struct Unit units[3 * 9];
static int population_count[1 << 9];


int cell_solved(int cell)
{
  return cell && !(cell & (cell - 1));
}

int cell_solution(int cell)
{
  int i = 1;
  while (cell /= 2)
    ++i;
  return i;
}

// Reads a number, skips non-digit characters
void read_number(FILE *file, int *n)
{
  if (fscanf(file, "%*[^0123456789]%d", n) != 1)
    fscanf(file, "%d", n);
}

void read_grid(int *grid, FILE *file)
{
  for (int i = 0; i < 9 * 9; ++i) {
    int n;
    read_number(file, &n);
    if (n == 0)
      grid[i] = 511;
    else
      grid[i] = 1 << (n - 1);
  }
}

void print_grid(int *grid)
{
  for (int i = 0; i < 9 * 9; ++i) {
    if (cell_solved(grid[i]))
      printf("%d", cell_solution(grid[i]));
    else
      putchar(' ');
    putchar((i + 1) % 9 ? ' ' : '\n');
  }
}

int grid_solved(int *grid)
{
  for (int i = 0; i < 3 * 9; ++i) {
    int unit_candidates = 0;
    for (int j = 0; j < 9; ++j) {
      int c = grid[units[i].cells[j]];
      if (!cell_solved(c))
        return 0;
      unit_candidates |= c;
    }
    if (unit_candidates != 511)
      return 0;
  }
  return 1;
}

int grid_valid(int *grid)
{
  for (int i = 0; i < 3 * 9; ++i) {
    int unit_candidates = 0;
    int unit_solved_candidates = 0;
    for (int j = 0; j < 9; ++j) {
      int c = grid[units[i].cells[j]];
      if (cell_solved(c)) {
        if (unit_solved_candidates & c)
          return 0;
        unit_solved_candidates |= c;
      }
      unit_candidates |= c;
    }
    if (unit_candidates != 511)
      return 0;
  }
  return 1;
}

int pass_naked_group(int *grid, int cell, const struct Unit *unit)
{
  // Find all cells who are not members of this group
  int nonmembers[9];
  int nonmembers_size = 0;
  for (int i = 0; i < 9; ++i) {
    if (grid[unit->cells[i]] & ~grid[cell])
      nonmembers[nonmembers_size++] = unit->cells[i];
  }
  // If the number of members is greater than or equal to the number of
  // candidates, this is a naked group
  if (9 - nonmembers_size < population_count[grid[cell]])
    return 0;
  // Remove candidates from nonmembers
  int num_candidates_removed = 0;
  for (int i = 0; i < nonmembers_size; ++i) {
    num_candidates_removed += population_count[grid[nonmembers[i]] & grid[cell]];
    grid[nonmembers[i]] &= ~grid[cell];
  }
  return num_candidates_removed;
}

int pass_hidden_group(int *grid, const struct Unit *unit)
{
  int num_candidates_removed = 0;
  // Build candidate -> cells bit map
  int candidate_cells[9] = {0};
  for (int i = 0; i < 9; ++i)
    for (int j = 0; j < 9; ++j)
      if (grid[unit->cells[j]] & (1 << i))
        candidate_cells[i] |= 1 << j;
  // Try all candidates
  for (int i = 0; i < 9; ++i) {
    int candidates = 0;
    // Find candidates whose cells are a subset
    for (int j = 0; j < 9; ++j)
      if ((candidate_cells[j] & ~candidate_cells[i]) == 0)
        candidates |= 1 << j;
    // If the number of cells for these candidates are less then or equal
    // to the number of candidates, this is a hidden group
    if (population_count[candidate_cells[i]] > population_count[candidates])
      continue;
    for (int j = 0; j < 9; ++j) {
      // Restrict candidates in cells
      if (candidate_cells[i] & (1 << j)) {
        num_candidates_removed += population_count[grid[unit->cells[j]] & ~candidates];
        grid[unit->cells[j]] &= candidates;
      }
      // Remove candidates from other cells
      else {
        num_candidates_removed += population_count[grid[unit->cells[j]] & candidates];
        grid[unit->cells[j]] &= ~candidates;
      }
    }
  }
  return num_candidates_removed;
}

int pass_intersection(int *grid, const struct Unit *unit)
{
  int num_candidates_removed = 0;
  // Try all intersecting units
  for (int u = 0; u < unit->num_intersections; ++u) {
    const struct Unit *int_unit = unit->intersections[u];
    // Check if any candidates intersect
    for (int i = 0; i < 9; ++i) {
      int cell_intersecting[9] = {0};
      int candidate_intersecting = 1;
      for (int j = 0; j < 9; ++j) {
        int cell_found = 0;
        for (int k = 0; k < 9; ++k)
          if (unit->cells[k] == int_unit->cells[j]) {
            cell_found = 1;
            cell_intersecting[k] = 1;
            break;
          }
        if (grid[int_unit->cells[j]] & (1 << i) && !cell_found) {
          candidate_intersecting = 0;
          break;
        }
      }
      if (!candidate_intersecting)
        continue;
      // Remove candidate from non-intersecting cells
      for (int j = 0; j < 9; ++j)
        if (!cell_intersecting[j] && grid[unit->cells[j]] & (1 << i)) {
          ++num_candidates_removed;
          grid[unit->cells[j]] &= ~(1 << i);
        }
    }
  }
  return num_candidates_removed;
}

// max_options specify the size limit of choices to attempt to solve
// depth specifices the maximum number of levels of recursion, where forks
// will be solved with choice union as well
// options = 3, depth = 2 (3^2) should be enough for any proper puzzle
// Forward declare solve_grid to resolve circular dependancy
int solve_grid(int *grid, int max_options, int depth);
int pass_choice_union(int *grid, const struct Unit *unit, int max_options, int depth)
{
  // Build candidate -> cells bit map
  int candidate_cells[9] = {0};
  for (int i = 0; i < 9; ++i)
    for (int j = 0; j < 9; ++j)
      if (grid[unit->cells[j]] & (1 << i))
        candidate_cells[i] |= 1 << j;
  // Find choice with at most N options
  for (int i = 0; i < 9; ++i) {
    int num_options = population_count[candidate_cells[i]];
    if (num_options < 2 || num_options > max_options)
      continue;
    // Create and solve forks
    // Using the stack should be fine
    Grid forks[num_options];
    int choice_cell[num_options];
    for (int j = 0, k = 0; j < 9; ++j)
      if (candidate_cells[i] & (1 << j))
        choice_cell[k++] = unit->cells[j];
    int choice = 0;
    for (int j = 0; j < num_options; ++j) {
      memcpy(forks[j], grid, sizeof(int) * 9 * 9);
      forks[j][choice_cell[choice++]] = 1 << i;
      solve_grid(forks[j], max_options, depth - 1);
    }
    // Join forks
    int num_candidates_removed = 0;
    for (int j = 0; j < 9 * 9; ++j) {
      int c = 0;
      for (int k = 0; k < num_options; ++k)
        c |= forks[k][j];
      num_candidates_removed += population_count[grid[j] & ~c];
      grid[j] = c;
    }
    // If progress was made, call it a job done
    if (num_candidates_removed > 0)
      return num_candidates_removed;
  }
  return 0;
}

// max_options and depth are forwarded to pass_choice_union
// Like all the logical solving functions,
// returns the number of candidates removed
int solve_grid(int *grid, int max_options, int depth)
{
  int total_candidates_removed = 0;
  int iteration_candidates_removed;
  do {
    iteration_candidates_removed = 0;
    for (int i = 0; i < 3 * 9; ++i) {
      for (int j = 0; j < 9; ++j)
        iteration_candidates_removed += pass_naked_group(grid, units[i].cells[j], &units[i]);
      iteration_candidates_removed += pass_hidden_group(grid, &units[i]);
      iteration_candidates_removed += pass_intersection(grid, &units[i]);
      if (iteration_candidates_removed == 0 && depth > 0)
        iteration_candidates_removed += pass_choice_union(grid, &units[i], max_options, depth);
    }
  } while (iteration_candidates_removed > 0);
  return total_candidates_removed;
}

// Returns whether or not a solution was reached
int solve_grid_with_guessing(int *grid)
{
  // Solve with logic as far as possible
  solve_grid(grid, 2, 0);
  if (grid_solved(grid))
    return 1;
  // Start guessing
  for (int i = 0; i < 9 * 9; ++i) {
    if (population_count[grid[i]] < 2)
      continue;
    for (int j = 0; j < 9; ++j) {
      if ((grid[i] & (1 << j)) == 0)
        continue;
      // Make a fork and try to solve it
      // Using the stack instead of the heap might be a
      // little risky here, but whatever
      Grid fork;
      memcpy(fork, grid, sizeof(int) * 9 * 9);
      fork[i] = 1 << j;
      if (solve_grid_with_guessing(fork)) {
        memcpy(grid, fork, sizeof(int) * 9 * 9);
        return 1;
      }
    }
  }
  return 0;
}


int main()
{
  // Initialize population count lookup table
  for (int i = 0; i < (1 << 9); ++i) {
    int n = 0;
    int j = i;
    while (j) {
      n += j & 1;
      j >>= 1;
    }
    population_count[i] = n;
  }

  // Initialize units
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      // Column, row, and box
      units[0 * 9 + i].cells[j] = j * 9 + i;
      units[1 * 9 + i].cells[j] = i * 9 + j;
      units[2 * 9 + i].cells[j] = i / 3 * 3 * 9 + i % 3 * 3 + j / 3 * 9 + j % 3;
    }
    // Column intersections (rows and boxes)
    for (int j = 0; j < 9; ++j) units[0 * 9 + i].intersections[0 + j] = &units[1 * 9 + j];
    for (int j = 0; j < 3; ++j) units[0 * 9 + i].intersections[9 + j] = &units[2 * 9 + j * 3 + i / 3];
    units[0 * 9 + i].num_intersections = 12;
    // Row intersections (columns and boxes)
    for (int j = 0; j < 9; ++j) units[1 * 9 + i].intersections[0 + j] = &units[0 * 9 + j];
    for (int j = 0; j < 3; ++j) units[1 * 9 + i].intersections[9 + j] = &units[2 * 9 + j + i / 3 * 3];
    units[1 * 9 + i].num_intersections = 12;
    // Box intersections (columns and rows)
    for (int j = 0; j < 3; ++j) units[2 * 9 + i].intersections[0 + j] = &units[0 * 9 + j + i % 3 * 3];
    for (int j = 0; j < 3; ++j) units[2 * 9 + i].intersections[3 + j] = &units[1 * 9 + j + i / 3 * 3];
    units[2 * 9 + i].num_intersections = 6;
  }

  FILE *puzzle = stdin;
  //FILE *puzzle = fopen("puzzle.txt", "r");
  int num_puzzles;
  read_number(puzzle, &num_puzzles);
  for (int i = 0; i < num_puzzles; ++i) {
    Grid grid;
    read_grid(grid, puzzle);
    printf("Puzzle #%d\n", i + 1);
    if (!grid_valid(grid)) {
      puts("Invalid grid\n");
      continue;
    }
    solve_grid(grid, 3, 2);
    if (grid_solved(grid))
      puts("Solved with logic");
    else {
      if (solve_grid_with_guessing(grid))
        puts("Solved with guessing");
      else
        puts("Not solved");
    }
    print_grid(grid);
    putchar('\n');
  }
  fclose(puzzle);
}
