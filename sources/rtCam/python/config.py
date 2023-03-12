import numpy as np
import yaml
import os


class Config(object):
	def __init__(self):
		super(Config, self).__init__()
		self._debug = False
		self._display_optional_windows = False
		self._display_duration = False
		self._detection_area = 500
		self._gaussian_blur = 0
		self._compute_gaussian_kernel()
		self._threshold = 70
		self._kernel_size = 6
		self._compute_kernel()
		self._rect_color = (0, 255, 0)  # BGR

	def get_debug(self):
		return self._debug

	def set_debug(self, value: bool):
		if not isinstance(value, bool):
			raise ValueError(f"debug must be a boolean. Given: {value}")
		self._debug = value
		if self._debug:
			self.set_display_optional_windows(True)
			self.set_display_duration(True)

	def get_display_optional_windows(self):
		return self._display_optional_windows

	def set_display_optional_windows(self, value: bool):
		if not isinstance(value, bool):
			raise ValueError(f"display optional windows must be a boolean. Given: {value}")
		self._display_optional_windows = value

	def get_display_duration(self):
		return self._display_duration

	def set_display_duration(self, value: bool):
		if not isinstance(value, bool):
			raise ValueError(f"display duration must be a boolean. Given: {value}")
		self._display_duration = value

	def inverse_display_duration(self):
		self.set_display_duration(not self._display_duration)

	def get_detection_area(self):
		return self._detection_area

	def _set_detection_area(self, value: int):
		if not isinstance(value, int) or value < 100 or value > 50000:
			raise ValueError(f"detection area must be an integer in [100;50000]. Given: {value}")
		self._detection_area = value

	def decrease_detection_area(self, decrement=100):
		if not isinstance(decrement, int) or decrement < 1:
			raise ValueError(f"decrement must be an integer greater than or equal to 1. Given: {decrement}")

		if self._detection_area - decrement >= 100:
			self._set_detection_area(self._detection_area - decrement)
			return True

		return False

	def increase_detection_area(self, increment=100):
		if not isinstance(increment, int) or increment < 1:
			raise ValueError(f"increment must be an integer greater than or equal to 1. Given: {increment}")

		if self._detection_area + increment <= 50000:
			self._set_detection_area(self._detection_area + increment)
			return True
			
		return False

	def get_gaussian_blur(self):
		return self._gaussian_blur

	def _set_gaussian_blur(self, value: int):
		if not isinstance(value, int) or value < 0 or value > 255 or (value != 0 and value % 2 == 0):
			raise ValueError(f"gaussian blur must be an odd integer in [0;255]. Given: {value}")
		self._gaussian_blur = value
		self._compute_gaussian_kernel()
	
	def decrease_gaussian_blur(self, decrement=2):
		if self._gaussian_blur == 1:
			decrement = 1

		if not isinstance(decrement, int) or decrement < 1:
			raise ValueError(f"decrement must be an integer greater than 1. Given: {decrement}")

		if self._gaussian_blur - decrement >= 0:
			self._set_gaussian_blur(self._gaussian_blur - decrement)
			return True

		return False

	def increase_gaussian_blur(self, increment=2):
		if self._gaussian_blur == 0:
			increment = 1

		if not isinstance(increment, int) or increment < 1:
			raise ValueError(f"increment must be an integer greater than 1. Given: {increment}")

		if self._gaussian_blur + increment <= 255:
			self._set_gaussian_blur(self._gaussian_blur + increment)
			return True
			
		return False

	def get_gaussian_kernel(self):
		return self._gaussian_kernel

	def _compute_gaussian_kernel(self):
		self._gaussian_kernel = (self._gaussian_blur, self._gaussian_blur)

	def get_threshold(self):
		return self._threshold

	def _set_threshold(self, value: int):
		if not isinstance(value, int) or value < 0 or value > 255:
			raise ValueError(f"threshold must be an integer in [0;255]. Given: {value}")
		self._threshold = value
	
	def decrease_threshold(self, decrement=1):
		if not isinstance(decrement, int) or decrement < 1:
			raise ValueError(f"decrement must be an integer greater than 1. Given: {decrement}")

		if self._threshold - decrement >= 0:
			self._set_threshold(self._threshold - decrement)
			return True

		return False

	def increase_threshold(self, increment=1):
		if not isinstance(increment, int) or increment < 1:
			raise ValueError(f"increment must be an integer greater than 1. Given: {increment}")

		if self._threshold + increment <= 255:
			self._set_threshold(self._threshold + increment)
			return True
			
		return False

	def get_kernel_size(self):
		return self._kernel_size

	def _set_kernel_size(self, value:int):
		if not isinstance(value, int) or value < 1 or value > 100:
			raise ValueError(f"kernel size must be an integer in [1;100]. Given: {value}")
		self._kernel_size = value
		self._compute_kernel()
	
	def decrease_kernel_size(self, decrement=1):
		if not isinstance(decrement, int) or decrement < 1:
			raise ValueError(f"decrement must be an integer greater than or equal to 1. Given: {decrement}")

		if self._kernel_size - decrement >= 1:
			self._set_kernel_size(self._kernel_size - decrement)
			return True

		return False

	def increase_kernel_size(self, increment=1):
		if not isinstance(increment, int) or increment < 1:
			raise ValueError(f"increment must be an integer greater than or equal to 1. Given: {increment}")

		if self._kernel_size + increment <= 100:
			self._set_kernel_size(self._kernel_size + increment)
			return True
			
		return False

	def get_kernel(self):
		return self._kernel

	def _compute_kernel(self):
		self._kernel = np.ones((self._kernel_size, self._kernel_size), np.uint8)

	def get_rectangle_color(self):
		return self._rect_color

	def _set_rect_color(self, b:int, g:int, r:int):
		if not isinstance(b, int) or b < 0 or b > 255:
			raise ValueError(f"blue must be an integer in [0;255]. Given: {b}")
		if not isinstance(g, int) or g < 0 or g > 255:
			raise ValueError(f"green color must be an integer in [0;255]. Given: {g}")
		if not isinstance(r, int) or r < 0 or r > 255:
			raise ValueError(f"red color must be an integer in [0;255]. Given: {r}")
		self._rect_color = (b, g, r)

	def display(self, sep='\t', start='\n', end='\n'):
		print(start, end="")
		print(f"gaussian blur: {self._gaussian_blur}{sep}", end="")
		print(f"threshold: {self._threshold}{sep}", end="")
		print(f"kernel size: {self._kernel_size}{sep}", end="")
		print(f"detection area: {self._detection_area}{sep}", end="")
		print(end, end="")

	def save(self):
		config = {
			"config": 
			{
				"debug": self._debug,
				"display optional windows": self._display_optional_windows,
			       "display duration": self._display_duration,
			       "detection area": self._detection_area,
				"gaussian blur": self._gaussian_blur,
			       "threshold" : self._threshold,
			       "kernel size": self._kernel_size,
			       "rectangle color": {
			       	"blue": self._rect_color[0],
			       	"green": self._rect_color[1],
			       	"red": self._rect_color[2]
			       }
			}
		}
		with open("config.yaml", 'w') as yamlfile:
		    yaml.dump(config, yamlfile)
		print("config saved as config.yaml")

	def read(self):
		if os.path.exists("config.yaml"):
			with open("config.yaml", 'r') as yamlfile:
				data = yaml.load(yamlfile, Loader=yaml.FullLoader)
				try:
					config = data["config"]
					color = config["rectangle color"]
					self.set_display_optional_windows(config["display optional windows"])
					self.set_display_duration(config["display duration"])
					# read after because debug changes previous parameters
					self.set_debug(config["debug"])
					self._set_detection_area(config["detection area"])
					self._set_gaussian_blur(config["gaussian blur"])
					self._set_threshold(config["threshold"])
					self._set_kernel_size(config["kernel size"])
					self._set_rect_color(color["blue"], color["green"], color["red"])

				except ValueError as valueError:
					print("Error in config.yaml file!")
					print(valueError.args[0])
					exit(-1)
				except KeyError as keyError:
					print("Error in config.yaml file!")
					print(f"Key not found: {keyError.args[0]}")
					print("config reset")
					self.reset()
					self.save()

		else:
			self.save()

	def reset(self):
		self.set_debug(False)
		self.set_display_optional_windows(False)
		self.set_display_duration(False)
		self._set_detection_area(500)
		self._set_gaussian_blur(0)
		self._set_threshold(70)
		self._set_kernel_size(6)
		self._set_rect_color(0, 255, 0)  # BGR
