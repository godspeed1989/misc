# --------------------------------------------------------------------
# An implementation of Kruskal's algorithm for generating mazes.
# Fairly expensive, memory-wise, as it requires memory proportional
# to the size of the entire maze, and it's not the fastest of the
# algorithms (what with all the set and edge management is has to
# do). Also, the mazes it generates tend to have a lot of very short
# dead-ends, giving the maze a kind of "spiky" look.
# --------------------------------------------------------------------
# NOTE: the display routine used in this script requires a terminal
# that supports ANSI escape sequences. Windows users, sorry. :(
# --------------------------------------------------------------------

# --------------------------------------------------------------------
# 1. Allow the maze to be customized via command-line parameters
# --------------------------------------------------------------------

width  = (ARGV[0] || 10).to_i
height = (ARGV[1] || width).to_i
seed   = (ARGV[2] || rand(0xFFFF_FFFF)).to_i
delay  = (ARGV[3] || 0.01).to_f

srand(seed)

# --------------------------------------------------------------------
# 2. Set up constants to aid with describing the passage directions
# --------------------------------------------------------------------

N, S, E, W = 1, 2, 4, 8
DX         = { E => 1, W => -1, N =>  0, S => 0 }
DY         = { E => 0, W =>  0, N => -1, S => 1 }
OPPOSITE   = { E => W, W =>  E, N =>  S, S => N }

# --------------------------------------------------------------------
# 3. Data structures and methods to assist the algorithm
# --------------------------------------------------------------------

def display_maze(grid)
  print "\e[H" # move to upper-left
  puts " " + "_" * (grid[0].length * 2 - 1)
  grid.each_with_index do |row, y|
    print "|"
    row.each_with_index do |cell, x|
      print "\e[47m" if cell == 0
      print((cell & S != 0) ? " " : "_")

      if cell & E != 0
        print(((cell | row[x+1]) & S != 0) ? " " : "_")
      else
        print "|"
      end
      print "\e[m" if cell == 0
    end
    puts
  end
end

class Tree
  attr_accessor :parent

  def initialize
    @parent = nil
  end

  def root
    @parent ? @parent.root : self
  end

  def connected?(tree)
    root == tree.root
  end

  def connect(tree)
    tree.root.parent = self
  end
end

grid = Array.new(height) { Array.new(width, 0) }
sets = Array.new(height) { Array.new(width) { Tree.new } }

# build the list of edges
edges = []
height.times do |y|
  width.times do |x|
    edges << [x, y, N] if y > 0
    edges << [x, y, W] if x > 0
  end
end

edges = edges.sort_by{rand}

# --------------------------------------------------------------------
# 4. Kruskal's algorithm
# --------------------------------------------------------------------

print "\e[2J" # clear the screen

until edges.empty?
  x, y, direction = edges.pop
  nx, ny = x + DX[direction], y + DY[direction]

  set1, set2 = sets[y][x], sets[ny][nx]

  unless set1.connected?(set2)
    display_maze(grid)
    sleep(delay)

    set1.connect(set2)
    grid[y][x] |= direction
    grid[ny][nx] |= OPPOSITE[direction]
  end
end

display_maze(grid)

# --------------------------------------------------------------------
# 5. Show the parameters used to build this maze, for repeatability
# --------------------------------------------------------------------

puts "#{$0} #{width} #{height} #{seed}"
