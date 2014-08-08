text_paths
{
  text : "neurotica efs"
}

noise_effectors
{
  apply_to : Position
  displacement : {
    type : 1
    keyframes: {
      time : 0
      value: { x: 0; y: 0; z: 250}
    }
    keyframes: {
      time : 2000
      value: { x: 10; y: 0; z: 1}
    }
    keyframes: {
      time : 3000
      value: { x: 0; y: 20; z: 1}
    }
    keyframes: {
      time : 5000
      value: { x: 0; y: 0; z: 1}
    }
  }
}