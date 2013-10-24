# --------------------------------------------------------------------
# An implementation of the "Binary Tree" algorithm. This is perhaps
# the simplest of the maze generation algorithms to implement, and the
# fastest to run, but it creates heavily biased mazes.
#
# It is novel in that it can operate without any state at all; it only
# needs to look at the current cell, without regard for the rest of
# the maze (or even the rest of the row). Thus, like Eller's algorithm
# it can be used to generate mazes of infinite size.
# --------------------------------------------------------------------

# --------------------------------------------------------------------
# 1. Allow the maze to be customized via command-line parameters
# --------------------------------------------------------------------

width  = (ARGV[0] || 10).to_i
height = (ARGV[1] || width).to_i
seed   = (ARGV[2] || rand(0xFFFF_FFFF)).to_i

srand(seed)

# --------------------------------------------------------------------
# 2. Set up constants to aid with describing the passage directions
# --------------------------------------------------------------------

N, S, E, W = 1, 2, 4, 8
DX         = { E => 1, W => -1, N =>  0, S => 0 }
DY         = { E => 0, W =>  0, N => -1, S => 1 }
OPPOSITE   = { E => W, W =>  E, N =>  S, S => N }

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
# 5. Binary Tree algorithm
# --------------------------------------------------------------------

print "\e[2J" # clear the screen

height.times do |y|
  width.times do |x|
    display_maze(grid)
    sleep 0.02

    dirs = []

    dirs << N if y > 0
    dirs << W if x > 0

    if (dir = dirs[rand(dirs.length)])
      nx, ny = x + DX[dir], y + DY[dir]
      grid[y][x] |= dir
      grid[ny][nx] |= OPPOSITE[dir]
    end
  end
end

display_maze(grid)

# --------------------------------------------------------------------
# 6. Show the parameters used to build this maze, for repeatability
# --------------------------------------------------------------------

puts "#{$0} #{width} #{height} #{seed}"