import string
import sys
import os


class array:
	def __init__( self, *dimensions ):
		self.dimensions = list( dimensions )
		print repr( self.dimensions )
		size = 1
		for dim in self.dimensions:
			size = size * dim
		print repr(size)
		self.data = [ 0 ] * size

	def _index( self, indices ):
		index = indices[0]
		for i in range(1,len(indices)):
			index = index * self.dimensions[i] + indices[i]
		return index

	def get( self, *indicies ):
		return self.data[ self._index( indicies ) ]

	def set( self, value, *indicies ):
		self.data[ self._index( indicies ) ] = value


# screen tuples are (modeline, drawcost, contents)

t0_screen = [
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(1, "mode line 4"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(1, "mode line 8"),
	(0, "minibuffer 5")]

t1_screen = [
	(0, "line A"),
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(0, "line 1"),
	(0, "line 2"),
	(1, "mode line 4"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line B"),
	(1, "mode line 8"),
	(0, "minibuffer 5")]

t2_screen = [
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(0, "line 1"),
	(0, "line 2"),
	(0, "line 3"),
	(1, "mode line 4"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(0, "line 5"),
	(0, "line 6"),
	(0, "line 7"),
	(1, "mode line 8"),
	(0, "minibuffer 5")]


def direction( new, old ):
	if old == -1:
		return '--'
	elif new > old:
		return 'vv %d' % (new - old)
	elif new == old:
		return '=='
	else:
		return '^^ %d' % (old - new)

_debug = 0
def updateScreen( new, old ):
	moves = []
	cost = 0
	old_start = 0
	for new_line in range( len(new) ):
		found = 0
		for old_line in range( old_start, len(old) ):
			cost  = cost + 1
			#print repr(new[new_line]),repr(old[old_line])
			# don't move mode lines
			if new[new_line][0] and old[old_line][0]:
				if _debug: print 'mode update %2d from %2d move %s' % (new_line, old_line, direction( new_line, old_line ) )
				old_start = old_line + 1
				moves.append( (new_line, old_line) )
				found = 1
				break
			if new[new_line][1] == old[old_line][1]:
				if _debug: print 'data update %2d from %2d move %s' % (new_line, old_line, direction( new_line, old_line ) )
				old_start = old_line + 1
				moves.append( (new_line, old_line) )
				found = 1
				break
		if not found:
			if _debug: print 'xxxx draw   %d' % new_line
			moves.append( (new_line, -1) )

	print '[cost %d of %d]' % (cost, (len(new)+1) * (len(old)+1))


	old_delta = 0
	old_delta_valid = 0

	for new_line, old_line in moves:
		delta = new_line - old_line
		if old_line == -1:
			delta_valid = 0
			detail = 'new %d old %d' %(new_line,old_line),
		else:
			delta_valid = new_line != old_line
			if delta_valid:
				detail = 'new %d old %d delta %d' %(new_line,old_line,delta),
			else:
				detail = 'new %d old %d' %(new_line,old_line),

		if old_delta == delta and delta_valid and old_delta_valid:
			if _debug: print detail, 'continue'
		else:
			if old_delta_valid:
				win = new_line - start_new_line
				print detail, 'end (%d-%d, %d-%d) win %d' % (start_new_line,start_new_line+win-1,start_old_line,start_old_line+win-1,win),
				old_delta_valid = 0
			if delta_valid:
				if _debug: print 'start'
				old_delta_valid = delta_valid
				old_delta = delta
				start_new_line = new_line
				start_old_line = old_line
			else:
				print detail,'huh'

def main():
	print 't1 from t0'
	updateScreen( t1_screen, t0_screen );
	print
	print 't2 from t1'
	updateScreen( t2_screen, t1_screen );



if __name__ == '__main__':
	main()

