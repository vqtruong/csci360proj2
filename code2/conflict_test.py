
from visualize import import_mapf_instance
import math

def get_distance(point1, point2):
	delta = [point1[i] - point2[i] for i in range(2)]
	distance = math.sqrt(delta[0]**2 + delta[1]**2)

	return distance

class Agent():

	_min_distance = 0.6
	_offset = 0.1

	def __init__(self, path, mapf, number):
		self.number = number
		self.path = path
		self.map = mapf
		self.position = path[0]
		self.prev_index = 0
		if len(path) <= 1:
			self.next_index = 0
			self.finished = True;
		else:
			self.next_index = 1
			self.finished = False

	def increment_time(self):
		self.position = self.path[self.next_index]
		self.prev_index = self.next_index

		if self.next_index + 1 < len(self.path):
			self.next_index += 1
		else:
			self.finished = True

	def move(self):

		if self.finished:
			return

		delta = [self._offset*(self.path[self.next_index][x]-self.path[self.prev_index][x]) for x in range(2)]
		self.position = [self.position[x] + delta[x] for x in range(2)]

	def collides(self, agent):

		distance = get_distance(agent.position, self.position)
		if distance < self._min_distance:
			#print("Agent {} at {} collided with agent {} at {} at a distance of {}".format(self.number, self.position, agent.number, agent.position, distance))
			return True
		else:
			return False


def all_finished(agents):
	finished = True
	for agent in agents:
		finished = finished and agent.finished
	return finished

def check_conflicts(paths, mapf):

	agents = []
	for i, path in enumerate(paths):
		agents.append(Agent(path, mapf, i))

	while not all_finished(agents):

		for i in range(10):
			for agent in agents:
				agent.move()

			for agent in agents:
				agents_copy = list(agents)
				agents_copy.remove(agent)
				for collision_agent in agents_copy:
					if agent.collides(collision_agent):
						return True

		for agent in agents:
			agent.increment_time()

	return False