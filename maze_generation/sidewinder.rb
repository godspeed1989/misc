# --------------------------------------------------------------------
# An implementation of the Sidewinder algorithm for maze generation.
# This algorithm is kind of a cross between the trivial Binary Tree
# algorithm, and Eller's algorithm. Like the Binary Tree algorithm,
# the result is biased (but not as heavily).
#
# Because the Sidewinder algorithm only needs to consider the current
# row, it can be used (like the Binary Tree and Eller's algorithms)
# to generate infinitely large mazes.
# --------------------------------------------------------------------

# --------------------------------------------------------------------
# 1. Allow the maze to be customized via command-line parameters
# --------------------------------------------------------------------

width  = (ARGV[0] || 10).to_i
height = (ARGV[1] || width).to_i
weight = (ARGV[2] || 2).to_i
seed   = (ARGV[3] || rand(0xFFFF_FFFF)).to_i

srand(seed)

# --------------------------------------------------------------------
# 2. Set up constants to aid with describing the passage directions
# --------------------------------------------------------------------

N, S, E, W = 1, 2, 4, 8

# --------------------------------------------------------------------
# 3. Data structures to assist the algorithm
# --------------------------------------------------------------------

grid = Array.new(height) { Array.new(width, 0) }

# --------------------------------------------------------------------
# 4. A simple routine to emit the maze as ASCII
# --------------------------------------------------------------------

def display_maze(grid)
  print "\e[H" # move to upper-left
  puts " " + "_" * (grid[0].length * 2 - 1)
  grid.each_with_index do |row, y|
    print "|"
    row.each_with_index do |cell, x|
      if cell == 0 && y+1 < grid.length && grid[y+1][x] == 0
        print " "
      else
        print((cell & S != 0) ? " " : "_")
      end

      if cell == 0 && x+1 < row.length && row[x+1] == 0
        print((y+1 < grid.length && (grid[y+1][x] == 0 || grid[y+1][x+1] == 0)) ? " " : "_")
      elsif cell & E != 0
        print(((cell | row[x+1]) & S != 0) ? " " : "_")
      else
        print "|"
      end
    end
    puts
  end
end

# --------------------------------------------------------------------
# 5. Sidewinder algorithm
# --------------------------------------------------------------------

print "\e[2J" # clear the screen

height.times do |y|
  run_start = 0
  width.times do |x|
    display_maze(grid)
    sleep 0.02

    if y > 0 && (x+1 == width || rand(weight) == 0)
      cell = run_start + rand(x - run_start + 1)
      grid[y][cell] |= N
      grid[y-1][cell] |= S
      run_start = x+1
    elsif x+1 < width
      grid[y][x] |= E
      grid[y][x+1] |= W
    end
  end
end

display_maze(grid)

# --------------------------------------------------------------------
# 6. Show the parameters used to build this maze, for repeatability
# --------------------------------------------------------------------

puts "#{$0} #{width} #{height} #{seed}"