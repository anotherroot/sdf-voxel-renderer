#type compute
#version 430
layout(local_size_x = 32, local_size_y = 18) in;
layout(rgba8, binding = 0) uniform image2D img_output;
layout(rgba8, binding = 1) uniform image2D img_depth;
layout(rgba8, binding = 2) uniform image2D img_comp;

layout( location=1 ) uniform vec3 u_color;  
layout( location=2 ) uniform mat4 u_view_projection;  
layout( location=3 ) uniform vec3 u_origin;  
layout( location=4 ) uniform vec3 u_vox_size;

layout( location=5 ) uniform sampler3D u_texture;
layout( location=6 ) uniform vec3 u_tex_size;
layout( location=7 ) uniform vec3 u_inv_fixed_size;
layout( location=8 ) uniform int u_perspective;
layout( location=9 ) uniform vec3 u_look_at;
layout( location=10 ) uniform int u_dist;

/* uniform vec3 u_color; */


struct AABB{
  vec3 tmin;
  vec3 tmax;
};
struct Ray{
  vec3 origin;
  vec3 dir;
};

AABB aabb(in Ray ray,in vec3 b_min, in vec3 b_max) {
  AABB o;

  float tx1 = (b_min.x - ray.origin.x)/ray.dir.x;
  float tx2 = (b_max.x - ray.origin.x)/ray.dir.x;

  o.tmin.x = min(tx1, tx2);
  o.tmax.x = max(tx1, tx2);

  float ty1 = (b_min.y - ray.origin.y)/ray.dir.y;
  float ty2 = (b_max.y - ray.origin.y)/ray.dir.y;

  o.tmin.y = min(ty1, ty2);
  o.tmax.y = max(ty1, ty2);

  float tz1 = (b_min.z - ray.origin.z)/ray.dir.z;
  float tz2 = (b_max.z - ray.origin.z)/ray.dir.z;

  o.tmin.z = min(tz1, tz2);
  o.tmax.z = max(tz1, tz2);

  return o;
}

struct Hit{
  bool hit;
  float dist;
  vec3 normal;
  vec3 color;
  float comp;
};
#define MAX_DIST 1000.0
vec4 GetVoxel(in vec3 tex_coords){
    return texture(u_texture,tex_coords);
}

Hit traverseDDA(in vec3 p0, in vec3 p1,  in vec3 tmin)
{
  Hit hit;
  hit.hit = false;
  hit.dist = 0;
  hit.normal = vec3(0);
  hit.comp=0;
  vec3 rd = p1 - p0; 
  vec3 p = clamp(floor(p0),vec3(0.0), u_tex_size-1);
  vec3 rd_inv = 1.0 / rd;
  vec3 stp = sign(rd);
	vec3 delta = min(rd_inv * stp, 1.0);
  vec3 t_max = abs((p + max(stp, vec3(0.0)) - p0) * rd_inv);
  hit.normal = step(tmin.zxy,tmin.xyz) * step(tmin.yzx,tmin.xyz)*stp;
  
  for (int i = 0; i < 512; ++i) {
    vec4 voxel = GetVoxel((p+0.5)*u_inv_fixed_size);
    voxel.a*=255.0;
    if( int(voxel.a) == int(0)){
        hit.color = voxel.rgb;
        /* hit.color = vec3(1.2); */
        /* hit.color = p0/u_tex_size; */
        /* hit.color = hit.normal*0.5+0.5; */
        hit.hit = true;
        break;
    }
    
    hit.comp+=1;
    /* voxel.a=1; */
    // visualizing SDF
    /* if(int(voxel.a)==u_dist){ */
    /*   hit.color = vec3(1,1,1); */
    /*   hit.hit=true; */
    /*   break; */
    /* } */
    /* voxel.a = min(voxel.a,1.0); */

    vec3 tmp_max = t_max + delta * vec3(voxel.a-1.0);
    hit.dist = min(min(tmp_max.x,tmp_max.y), tmp_max.z);
    
    // change to see it it affects the noise
    vec3 t = step(tmp_max.xyz,tmp_max.zxy) * step(tmp_max.xyz,tmp_max.yzx);
    if(hit.dist>0.999){
      break;
    }


    hit.normal = t*stp;


    vec3 p2 = floor(p0+ (hit.dist) * rd+0.1*hit.normal)  ;
    vec3 cmp = abs(p2-p);// length of  steps in each voxel direction
    vec3 cmpm = vec3(voxel.a-1)+t;
    cmp = min(cmp,cmpm);

    // old DDA step 
    /* vec3 cmp = t; */

    t_max += delta * cmp;
    p += stp * cmp ;
  }  
  hit.dist*=length(rd);
  hit.normal *=-1;
  return hit;
}
  
Hit traverse(in Ray ray, in float tmax, in vec3 tmin){
  vec3 b_size = u_tex_size;
  vec3 p0 = ray.origin/u_vox_size;
  vec3 p1 = (ray.origin+ray.dir*tmax)/u_vox_size;
  Hit hit =traverseDDA(p0,p1,tmin); 
  hit.dist*=u_vox_size.r;
  return hit;
}








 /* check intersection to bounding box */
Hit intersect(in Ray ray){
  Hit hit;
  hit.hit =false;
  hit.dist = MAX_DIST;
  hit.comp=0;


  // intersection AABB
  vec3 b_min = -vec3(0.5,0,0.5)*u_vox_size*u_tex_size;
  vec3 b_max = vec3(0.5,1,0.5)*u_vox_size*u_tex_size;
  /* vec3 b_min = vec3(0); */
  /* vec3 b_max = u_vox_size*u_tex_size; */
  AABB ab = aabb(ray,b_min, b_max);
  float tmax =min(ab.tmax.x,min(ab.tmax.z,ab.tmax.y)); 
  float tmin =max(ab.tmin.x,max(ab.tmin.z,ab.tmin.y)); 
  float t = max(0.0,tmin);  

  if( tmax>t&&t<hit.dist){
    ray.origin +=t*ray.dir;
    ray.origin -=b_min;

    /* //TODO: temp */
    /* hit.hit=true; */
    /* hit.dist = t; */
    /* hit.color = ivec3(ray.origin/u_vox_size)*u_inv_fixed_size; */ 
    /* return hit; */

    hit = traverse(ray,tmax-t,ab.tmin);
    hit.dist+=t;
    hit.hit =hit.hit&&hit.dist<MAX_DIST;
  }
  return hit;
}


void main() {
  // base pixel colour for image
  // get index in global work group i.e x,y position
  /* ivec2 max_coords = ivec2(gl_NumWorkGroups.xy*gl_WorkGroupSize.xy); */
  ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy+0.01);
  vec4 pixel = vec4(u_color,1);
  float depth = 1;
  Ray ray;
  ray.origin = u_origin; 
  vec2 uv = vec2((gl_GlobalInvocationID.xy/vec2(gl_NumWorkGroups.xy*gl_WorkGroupSize.xy))*2-1);
  // get ray directon
    vec4 wp = inverse(u_view_projection)*vec4(uv,0.0,1);
    wp/=wp.w;
  if(u_perspective ==int(1)){
    ray.dir = normalize(vec3(wp) - ray.origin);
  /* pixel = vec4(uv,1,1); */
  /*   imageStore(img_output, pixel_coords, pixel); */
  /*   return; */
  }
  else{
    vec3 r = normalize(cross(vec3(0,1,0),u_look_at));

    ray.origin = u_origin +dot(r,wp.xyz)/100.0*r+dot(vec3(0,1,0),wp.xyz)/100.0*vec3(0,1,0);
    ray.dir = u_look_at;
  }
  Hit hit = intersect(ray);
  pixel = vec4((normalize(ray.dir)*0.5+0.5)/8.0,1);
  /* pixel = vec4(0,0,0,1); */
  if(hit.hit){
      /* float diff = (1+dot(normalize(vec3(1,3,2)),hit.normal))/2.0; */
      float diff = 1;
    pixel = vec4(hit.color*diff,1);
    /* pixel = vec4(hit.color,1); */
    depth = (hit.dist)/MAX_DIST;


    /* pixel = vec4(hit.color,1); */
  }



  float cm = min(hit.comp/100,1);
  
  vec4 comp_col = cm*vec4(1,0,0,1)+(1-cm)*vec4(0,1,0,1);
  // output to a specific pixel in the image
  imageStore(img_output, pixel_coords, pixel);
  imageStore(img_depth, pixel_coords, vec4(vec3(depth),1));
  imageStore(img_comp, pixel_coords, comp_col);

  /* ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);\ */
 /* float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);\ */
 /* float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 +u_roll)*0.5;\ */
 /* imageStore(img_output, storePos, vec4(1.0-globalCoef*localCoef, 0.0, 0.0, 0.0));\ */

}

