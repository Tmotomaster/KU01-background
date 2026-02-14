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
  # path = input("What is the path to the desired image: ")
  # raw_img = Image.open(path, "r")
  # img = raw_img.convert("RGB")
  # ImageOps.exif_transpose(img)
  
  data = img.getdata()
  width = img.width
  height = img.height

  datafile = open("_srcdata.txt", "w", encoding="utf-8")
  datafile.write(f"{width} {height}\n")
  for d in data:
    datafile.write(f"{d[0]} {d[1]} {d[2]}\n")
  datafile.close()

if __name__ == "__main__":
  main()