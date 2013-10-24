# --------------------------------------------------------------------
# An implementation of the "Hunt and Kill" algorithm. This is fairly
# similar to the recursive backtracking algorithm, except that there
# is no recursion, and it doesn't backtrack. :) The algorithm can
# get a little slow towards the end, where the "hunt" phase has to
# search over nearly the entire field to find a candidate cell, but
# it's guaranteed to finish (unlike Aldous-Broder and Wilson's), and
# it's still pretty fast.
# --------------------------------------------------------------------

# --------------------------------------------------------------------
# 1. Allow the maze to be customized via command-line parameters
# --------------------------------------------------------------------

width  = (ARGV[0] || 10).to_i
height = (ARGV[1] || width).to_i
seed   = (ARGV[2] || rand(0xFFFF_FFFF)).to_i

srand(seed)

grid = Array.new(height) { Array.new(width, 0) }

# --------------------------------------------------------------------
# 2. Set up constants to aid with describing the passage directions
# --------------------------------------------------------------------

N, S, E, W = 1, 2, 4, 8
IN         = 0x10
DX         = { E => 1, W => -1, N =>  0, S => 0 }
DY         = { E => 0, W =>  0, N => -1, S => 1 }
OPPOSITE   = { E => W, W =>  E, N =>  S, S => N }

# --------------------------------------------------------------------
# 3. A simple routine to emit the maze as ASCII
# --------------------------------------------------------------------

def display_maze(grid, cy=nil)
  print "\e[H" # move to upper-left
  puts " " + "_" * (grid[0].length * 2 - 1)
  grid.each_with_index do |row, y|
    print "|"
    row.each_with_index do |cell, x|
      print "\e[43m" if cy == y # cursor is yellow
      if cell == 0 && y+1 < grid.length && grid[y+1][x] == 0
        print " "
      else
        print((cell & S != 0) ? " " : "_")
      end
      print "\e[0m" if cy == y

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
# 4. Hunt and Kill algorithm
# --------------------------------------------------------------------

def walk(grid, x, y)
  [N, S, E, W].shuffle.each do |dir|
    nx, ny = x + DX[dir], y + DY[dir]
    if nx >= 0 && ny >= 0 && ny < grid.length && nx < grid[ny].length && grid[ny][nx] == 0
      grid[y][x] |= dir
      grid[ny][nx] |= OPPOSITE[dir]

      return [nx, ny]
    end
  end

  nil
end

def hunt(grid)
  grid.each_with_index do |row, y|
    display_maze(grid, y)
    sleep 0.02

    row.each_with_index do |cell, x|
      next unless cell == 0
      neighbors = []
      neighbors << N if y > 0 && grid[y-1][x] != 0
      neighbors << W if x > 0 && grid[y][x-1] != 0
      neighbors << E if x+1 < grid[y].length && grid[y][x+1] != 0
      neighbors << S if y+1 < grid.length && grid[y+1][x] != 0

      direction = neighbors[rand(neighbors.length)] or next
      nx, ny = x + DX[direction], y + DY[direction]

      grid[y][x] |= direction
      grid[ny][nx] |= OPPOSITE[direction]

      return [x, y]
    end
  end

  nil
end

print "\e[2J" # clear the screen

x, y = rand(width), rand(height)

loop do
  display_maze(grid)
  sleep 0.02

  x, y = walk(grid, x, y)
  x, y = hunt(grid) unless x
  break unless x
end

display_maze(grid)

# --------------------------------------------------------------------
# 5. Show the parameters used to build this maze, for repeatability
# --------------------------------------------------------------------

puts "#{$0} #{width} #{height} #{seed}"
