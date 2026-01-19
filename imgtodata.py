from PIL import Image, ImageOps

def main():
  path = input("What is the path to the desired image: ")
  raw_img = Image.open(path, "r")
  img = raw_img.convert("RGB")
  ImageOps.exif_transpose(img)
  
  data = img.getdata()
  width = img.width
  height = img.height
  raw_img.close()

  datafile = open("_srcdata.txt", "w", encoding="utf-8")
  datafile.write(f"{width} {height}\n")
  for d in data:
    datafile.write(f"{d[0]} {d[1]} {d[2]}\n")
  datafile.close()

if __name__ == "__main__":
  main()