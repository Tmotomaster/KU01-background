from PIL import Image, ImageOps

def ask_for_img():
  path = input("What is the path to the desired image: ")
  if path == "":
    return None
  try:
    raw_img = Image.open(path, "r")
    img = raw_img.convert("RGB")
    ImageOps.exif_transpose(img)
    raw_img.close()
    return img
  except OSError:
    print("Error loading image. Please try again.")
    return ask_for_img()

def main():
  img = ask_for_img()
  if img == None:
    return
  
  data = img.getdata()
  width = img.width
  height = img.height

  datafile = open("_srcdata.ppm", "wb")
  datafile.write(f"P6 {width} {height}".encode("utf-8") + b"\n255\n")
  for d in data:
    datafile.write(d[0].to_bytes(1))
    datafile.write(d[1].to_bytes(1))
    datafile.write(d[2].to_bytes(1))
  datafile.close()

if __name__ == "__main__":
  main()