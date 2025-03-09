import os
import sys

import cv2
import numpy as np
import bpy
import bmesh
import json

BOT_ARM_TRANS = [-0.072266 - 0.006897, 0.085937 + 0.009853]
TOP_ARM_TRANS = [0.043945, 0.006836]
RIGHT_LEG_TRANS = -0.014389
BODY_TRANS = [0.0, 0.170266]
LIP_TRANS = [1/4+1/16, -1/32]
LEFT_LEG_COLOR = (64 * 256 + 24) * 256 + 126
RIGHT_LEG_COLOR = (42 * 256 + 126) * 256 + 24
BOT_ARM_COLOR = (62 * 256 + 21) * 256 + 211
LIP_COLOR = (21 * 256 + 109) * 256 + 211
TOP_ARM_COLOR = (255 * 256 + 0) * 256 + 0
BODY_COLOR = (21 * 256 + 211) * 256 + 91
HEAD_COLOR = (204 * 256 + 162) * 256 + 20



def select_bone(bone):
    bone.select = True
    bone.select_head = True
    bone.select_tail = True


def find_body_rig():
    o = bpy.data.objects[0]
    while o.parent is not None:
        o = o.parent
    return o


class DazOptimizer:

    def __init__(self, workdir=None, name=None):
        if workdir is None:
            workdir = os.path.dirname(bpy.data.filepath)
        if name is None:
            name = os.path.basename(bpy.data.filepath)
            name = name[:name.rindex('.')]
        self.name = name
        self.workdir = workdir
        self.body_mesh = None
        self.body_rig = None

    def gold_palace_dir(self):
        return os.path.join(self.workdir, "textures/original/meipex/m_goldenpalace/g9")

    def us_mask_path(self):
        return os.path.join(self.workdir, "uv_region_mask.png")

    def textures_dir(self):
        return os.path.join(self.workdir, "textures")

    def genesis_dir(self):
        return os.path.join(self.workdir, "textures/original/daz/characters/genesis9")

    def get_uv_mask(self):
        from PIL import Image
        uv_region_mask = np.array(Image.open(self.us_mask_path()), dtype=np.uint32)
        uv_region_mask = (uv_region_mask[:, :, 0] * 256 + uv_region_mask[:, :, 1]) * 256 + uv_region_mask[:, :, 2]
        return uv_region_mask

    def find_body(self):
        self.body_rig = find_body_rig()
        self.body_mesh = bpy.data.objects[self.body_rig.name+' Mesh']

    def get_body_mesh(self):
        if self.body_mesh is None:
            self.find_body()
        return self.body_mesh

    def get_body_rig(self):
        if self.body_rig is None:
            self.find_body()
        return self.body_rig

    def get_eyes_mesh(self):
        return bpy.data.objects['Genesis 9 Eyes Mesh']

    def get_base_uv_layer(self):
        return self.get_body_mesh().data.uv_layers['Base Multi UDIM']

    def get_base_uv_layer_np(self):
        return np.array([v.uv for v in self.get_base_uv_layer().data])

    def get_base_uv_layer_selection_np(self):
        return np.array([v.select for v in self.get_base_uv_layer().data], dtype=bool)

    def update_base_uv_layer(self, base_layer_np: np.ndarray):
        for v, new_uv in zip(self.get_base_uv_layer().data, base_layer_np):
            v.uv = new_uv

    def optimize_eyes(self):
        EYES_M = self.get_eyes_mesh()
        bpy.ops.object.select_all(action='DESELECT')
        EYES_M.select_set(True)
        bpy.context.view_layer.objects.active = EYES_M

        #uv_layer = EYES_M.data.uv_layers.active
        #uvs = np.array([v.uv for v in uv_layer.data], dtype=bool)
        #uvs[:, y] < 0.5
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.context.scene.tool_settings.use_uv_select_sync = False
        bpy.ops.uv.select_all(action='DESELECT')
        bpy.ops.mesh.select_all(action='DESELECT')

        me = bpy.context.object.data
        bm = bmesh.from_edit_mesh(me)
        uv_layer = bm.loops.layers.uv.verify()

        for v in bm.verts:
            v.select = False

        for face in bm.faces:
            for loop in face.loops:
                loop_uv = loop[uv_layer]
                uv = np.array(loop_uv.uv)
                dist = np.linalg.norm(uv % 0.5 - 0.25)
                if uv[1] < 0.5 or dist > 0.24 or dist < 0.038:
                    loop.vert.select = True
        for v in bm.verts:
            if v.select:
                bm.verts.remove(v)

        def select_loop(center):
            for face in bm.faces:
                for loop in face.loops:
                    loop_uv = loop[uv_layer]
                    uv = np.array(loop_uv.uv)
                    dist = np.linalg.norm(uv - center)
                    loop.vert.select_set(dist < 0.044)

        bmesh.update_edit_mesh(me)

        select_loop([0.25, 0.75])
        bpy.ops.mesh.edge_face_add()
        select_loop([0.75, 0.75])
        bpy.ops.mesh.edge_face_add()

        bmesh.update_edit_mesh(me)

        bpy.ops.object.mode_set(mode='OBJECT')
        #

    def concat_textures(self):
        from PIL import Image
        import re

        name_pattern = re.compile(
            r"([A-Za-z0-9_-]+)_([Hh]ead|[Ee]yelashes|[Ll]egs|[Nn]ails|[Bb]ody|[Aa]rms|[Mm]outh|[Ee]yes_sclera[_0-9]*|[Ee]yes_iris[_0-9]*)[A-Za-z]?(_(D|NM|R|SSS|C|nm)(_[0-9]+)?)?\.(jpg|png)")

        img_file_paths = {}
        gp_map_types = {
            "Roughness": "R",
            "NormalMap": "NM",
            "Speculairty": "SSS",
            "Color": "D",
        }
        for root, dirs, files in os.walk(self.textures_dir()):
            for file_name in files:
                body_part = None
                map_type = None
                if file_name.startswith("G9GP_") and file_name.endswith(".jpg"):
                    map_type = file_name[len("G9GP_"):-len(".jpg")]
                    map_type = gp_map_types.get(map_type)
                    body_part = "G9GP"
                else:
                    match = name_pattern.fullmatch(file_name)
                    if match:
                        object_name = match.group(1)
                        body_part = match.group(2).lower()
                        map_type = match.group(4)
                        if map_type is None or map_type == '':
                            map_type = 'D'
                        else:
                            map_type = map_type.rstrip('_')
                        body_part = body_part.rstrip('_0123456789')
                if body_part is not None and map_type is not None:
                    file_path = os.path.join(root, file_name)
                    if body_part not in img_file_paths:
                        img_file_paths[body_part] = {}
                    img_file_paths[body_part][map_type] = file_path

        uv_region_mask = self.get_uv_mask()
        MASK_TILE_SIZE = uv_region_mask.shape[0] // 2
        arms_region_mask = uv_region_mask[:MASK_TILE_SIZE, MASK_TILE_SIZE:]
        body_region_mask = uv_region_mask[MASK_TILE_SIZE:, MASK_TILE_SIZE:]
        legs_region_mask = uv_region_mask[:MASK_TILE_SIZE, :MASK_TILE_SIZE]
        head_region_mask = uv_region_mask[MASK_TILE_SIZE:, :MASK_TILE_SIZE]

        print(json.dumps(img_file_paths, indent=4))
        # from matplotlib import pyplot as plt
        for map_type in ["D", "R", "NM", "SSS"]:
            print("map_type=", map_type)
            head_file: str = img_file_paths['head'][map_type]
            _, extension = head_file.rsplit('.', maxsplit=1)
            head_tile = Image.open(head_file)
            body_tile = Image.open(img_file_paths['body'][map_type])
            arms_tile = Image.open(img_file_paths['arms'][map_type])
            legs_tile = Image.open(img_file_paths['legs'][map_type])

            head_tile = np.array(head_tile)
            body_tile = np.array(body_tile)
            arms_tile = np.array(arms_tile)
            legs_tile = np.array(legs_tile)

            s = legs_tile.shape[0]
            s2 = s * 2
            if ('eyes_iris' in img_file_paths and 'eyes_sclera' in img_file_paths
                    and map_type in img_file_paths['eyes_iris'] and map_type in img_file_paths['eyes_sclera']):
                iris = Image.open(img_file_paths['eyes_iris'][map_type])
                sclera = Image.open(img_file_paths['eyes_sclera'][map_type])
                iris = np.array(iris)
                iris_rgb = iris[:,:,:3]
                iris_a = iris[:, :, 3]
                if iris_a.dtype.kind != 'f':
                    iris_a = iris_a/255
                sclera = np.array(sclera)
                sclera_h, sclera_w = sclera.shape[:2]
                sclera_h = sclera_h // 2
                sclera = sclera[:sclera_h]
                iris_h, iris_w = s // 8, s // 4
                eyes = iris_rgb.T * iris_a.T + sclera.T * (1-iris_a.T)
                eyes = eyes.T
                eyes = cv2.resize(eyes, [iris_w, iris_h])

            else:
                eyes = None

            if map_type != "NM":
                nails_tile = Image.open(img_file_paths['nails'][map_type])
                nails_tile = np.array(nails_tile)
                nails_tile_size = s2//8
                if map_type == "R":
                    nails_tile = np.average(nails_tile, axis=2)
                    nails_tile = nails_tile.astype(legs_tile.dtype)
                nails_tile = cv2.resize(nails_tile, (nails_tile_size, nails_tile_size))

            else:
                nails_tile = None
            if "G9GP" in img_file_paths and map_type in img_file_paths["G9GP"]:
                gp_tile = Image.open(img_file_paths['G9GP'][map_type])
                gp_tile = np.array(gp_tile)
                gp_tile_size = s2 * 14 // 64
                if map_type == "R":
                    gp_tile = np.average(gp_tile, axis=2)
                    gp_tile = gp_tile.astype(legs_tile.dtype)
                gp_tile = cv2.resize(gp_tile, (gp_tile_size, gp_tile_size))
            else:
                gp_tile = None

            def shift_img(img: np.ndarray, y0, y1, x0, x1, mask: np.ndarray, translation: [float, float], hflip=False):
                shape = [s2, s2, legs_tile.shape[2]] if len(legs_tile.shape) > 2 else [s2, s2]
                new_img = np.zeros(shape, dtype=legs_tile.dtype)
                if hflip:
                    mask = np.flipud(mask)
                    img = np.flipud(img)
                new_img[y0:y1, x0:x1][mask] = img[mask]
                x, y = np.int32(np.array(translation) * s2)
                if x != 0 or y != 0:
                    new_img = np.roll(new_img, [-y, x], axis=[0, 1])
                return new_img

            # Textures are concatenated as follows:
            #   Legs | Arms
            #  ------+-----
            #   Head | Body
            packed = shift_img(arms_tile, 0, s, s, s2, arms_region_mask == BOT_ARM_COLOR, BOT_ARM_TRANS)
            packed += shift_img(arms_tile, 0, s, s, s2, arms_region_mask == TOP_ARM_COLOR, TOP_ARM_TRANS)
            packed += shift_img(legs_tile, 0, s, 0, s, legs_region_mask == LEFT_LEG_COLOR, [0, 0])
            packed += shift_img(legs_tile, 0, s, 0, s, legs_region_mask == RIGHT_LEG_COLOR, [RIGHT_LEG_TRANS, 0], True)
            packed += shift_img(body_tile, s, s2, s, s2, body_region_mask == BODY_COLOR, BODY_TRANS)
            packed += shift_img(head_tile, s, s2, 0, s, head_region_mask == LIP_COLOR, LIP_TRANS)
            # packed += shift_img(head, s, s2, s, s2, head_region_mask == HEAD_COLOR, [0.008526, 0.019377])
            packed[s:, :s] = head_tile
            if nails_tile is not None:
                packed[s2-nails_tile_size:s2, s:s+nails_tile_size] = nails_tile
            if gp_tile is not None:
                packed[s2 - gp_tile_size:s2, s2-gp_tile_size:s2] += gp_tile
            if eyes is not None:
                packed[s2 - s//4-s//8:s2-s//4, s+s//4:s+s//2] = eyes

            # packed[:s, :s] = legs_tile
            # packed[s:, s:] = body_tile
            # packed[:s, s:] = arms_tile
            packed = Image.fromarray(packed)
            packed.save(os.path.join(self.workdir, self.name + '_' + map_type + '.png'))
            # plt.imshow(packed)
            # plt.show()


    def merge_geografts(self):
        BODY_M = self.get_body_mesh()
        BODY_RIG = self.get_body_rig()

        # merge meshes
        bpy.ops.object.select_all(action='DESELECT')
        GOLD_PAL_RIG = None
        if 'GoldenPalace_G9' in bpy.data.objects:
            GOLD_PAL_M = bpy.data.objects['GoldenPalace_G9 Mesh']
            GOLD_PAL_RIG = bpy.data.objects['GoldenPalace_G9']
            GOLD_PAL_M.select_set(True)
        BREASTACULAR_RIG = None
        if 'BreastacularG9' in bpy.data.objects:
            BREASTACULAR_M = bpy.data.objects['BreastacularG9 Mesh']
            BREASTACULAR_RIG = bpy.data.objects['BreastacularG9']
            BREASTACULAR_M.select_set(True)
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.daz.merge_geografts()

        # merge bones
        if GOLD_PAL_RIG is not None:
            self.merge_two_rigs(BODY_RIG, GOLD_PAL_RIG)
        if BREASTACULAR_RIG is not None:
            self.merge_two_rigs(BODY_RIG, BREASTACULAR_RIG)

    def merge_two_rigs(self, original, addon):
        bpy.ops.object.select_all(action='DESELECT')
        addon.select_set(True)
        bpy.context.view_layer.objects.active = original
        bpy.ops.object.mode_set(mode='OBJECT')

        deform_bones = {bone.name for bone in addon.data.bones if bone.use_deform}
        parents = {bone.name: bone.parent.name for bone in addon.data.bones if bone.parent is not None}

        bpy.ops.object.mode_set(mode='EDIT')
        duplicates = [bone for bone in addon.data.edit_bones if bone.name in original.data.bones]
        for dup in duplicates:
            addon.data.edit_bones.remove(dup)

        addon.select_set(True)
        original.select_set(True)
        bpy.context.view_layer.objects.active = original
        bpy.ops.object.mode_set(mode='OBJECT')
        bpy.ops.object.join()
        for bone in original.data.bones:
            if bone.name in deform_bones:
                bone.use_deform = True
        bpy.ops.object.mode_set(mode='EDIT')
        for bone in original.data.edit_bones:
            parent = parents.get(bone.name)
            if parent is not None:
                bone.parent = original.data.edit_bones[parent]
        bpy.ops.object.mode_set(mode='OBJECT')

    def merge_eyes(self):

        EYES_M = bpy.data.objects['Genesis 9 Eyes Mesh']
        BODY_M = self.get_body_mesh()
        EYES_RIG = bpy.data.objects['Genesis 9 Eyes']
        BODY_RIG = self.get_body_rig()

        uv_map_name = EYES_M.data.uv_layers.active.name

        # merge meshes
        bpy.ops.object.select_all(action='DESELECT')
        EYES_M.select_set(True)
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.join()

        # merge UV maps
        eyes_layer = BODY_M.data.uv_layers[uv_map_name]
        eyes_layer_np = np.array([v.uv for v in eyes_layer.data])
        is_eye = np.all(eyes_layer_np > 0, axis=1)
        base_layer_np = self.get_base_uv_layer_np()
        base_layer_np[is_eye] = eyes_layer_np[is_eye] + [5,0]
        self.update_base_uv_layer(base_layer_np)
        BODY_M.data.uv_layers.remove(eyes_layer)

        # merge bones
        self.merge_two_rigs(BODY_RIG, EYES_RIG)

    def merge_mouth(self):

        MOUTH_M = bpy.data.objects['Genesis 9 Mouth Mesh']
        BODY_M = self.get_body_mesh()
        MOUTH_RIG = bpy.data.objects['Genesis 9 Mouth']
        BODY_RIG = self.get_body_rig()

        uv_map_name = MOUTH_M.data.uv_layers.active.name

        # merge meshes
        bpy.ops.object.select_all(action='DESELECT')
        MOUTH_M.select_set(True)
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.join()

        # merge UV maps
        mouth_layer = BODY_M.data.uv_layers[uv_map_name]
        mouth_layer_np = np.array([v.uv for v in mouth_layer.data])
        is_mouth = np.all(mouth_layer_np > 0, axis=1)
        base_layer_np = self.get_base_uv_layer_np()
        base_layer_np[is_mouth] = mouth_layer_np[is_mouth] + [6,0]
        self.update_base_uv_layer(base_layer_np)
        BODY_M.data.uv_layers.remove(mouth_layer)

        # merge bones
        self.merge_two_rigs(BODY_RIG, MOUTH_RIG)

    def pack_uvs(self):

        # ========= Concat UVs =========
        BODY_M = self.get_body_mesh()
        # pack UVs
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.mode_set(mode='OBJECT')
        base_layer_np = self.get_base_uv_layer_np()
        is_nails = np.logical_and(4 < base_layer_np[:, 0], base_layer_np[:, 0] < 5)
        is_eyes = np.logical_and(5 < base_layer_np[:, 0], base_layer_np[:, 0] < 6)
        is_mouth = np.logical_and(6 < base_layer_np[:, 0], base_layer_np[:, 0] < 7)
        base_layer_np *= 0.5
        out_of_bounds = base_layer_np[:, 0] > 1
        base_layer_np[out_of_bounds] += [-1, 0.5]
        gp_layer = BODY_M.data.uv_layers['Golden Palace']
        gp_layer_np = np.array([v.uv for v in gp_layer.data])
        is_gp = np.all(gp_layer_np > 0, axis=1)



        uv_mask = self.get_uv_mask()
        uv_mask_size = uv_mask.shape[0]

        def get_pixel_class():
            pixel_coords = np.copy(base_layer_np)
            pixel_coords[:, 1] = 1 - pixel_coords[:, 1]
            pixel_coords = (pixel_coords * uv_mask_size).clip(0, uv_mask_size - 1)
            pixel_coords = np.int32(pixel_coords)
            pix_cls = uv_mask[pixel_coords[:, 1], pixel_coords[:, 0]]
            return pix_cls

        pixel_class = get_pixel_class()

        base_layer_np[pixel_class == BOT_ARM_COLOR] += BOT_ARM_TRANS
        base_layer_np[pixel_class == TOP_ARM_COLOR] += TOP_ARM_TRANS
        base_layer_np[pixel_class == RIGHT_LEG_COLOR, 1] = 1.5 - base_layer_np[pixel_class == RIGHT_LEG_COLOR, 1]
        base_layer_np[pixel_class == RIGHT_LEG_COLOR, 0] += RIGHT_LEG_TRANS
        base_layer_np[pixel_class == BODY_COLOR] += BODY_TRANS
        base_layer_np[is_nails] = base_layer_np[is_nails] / 4 + [0.25, -0.5 / 4]
        base_layer_np[is_eyes] = base_layer_np[is_eyes] / 4 + [-6/16 + 0.5 + 1/8, - 0.5 / 8]
        base_layer_np[is_gp] = gp_layer_np[is_gp] * 14 / 64 + [(64-14) / 64, 0]
        base_layer_np[is_mouth] = base_layer_np[is_mouth] / 4 + [1/8, -0.5 / 4]
        self.update_base_uv_layer(base_layer_np)

    def separate_lip_uvs(self):
        BODY_M = self.get_body_mesh()
        # pack UVs
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.mode_set(mode='EDIT')

        uv_mask = self.get_uv_mask()
        uv_mask_size = uv_mask.shape[0]

        bpy.context.scene.tool_settings.use_uv_select_sync = False
        bpy.ops.uv.select_all(action='DESELECT')
        bpy.ops.mesh.select_all(action='DESELECT')

        me = bpy.context.object.data
        bm = bmesh.from_edit_mesh(me)
        uv_layer = bm.loops.layers.uv.verify()

        # for v in bm.verts:
        #    v.select_set(False)

        for face in bm.faces:
            full_loop = True
            for loop in face.loops:
                loop_uv = loop[uv_layer]
                uv = np.array(loop_uv.uv)
                uv[1] = 1 - uv[1]
                pixel_coord = (uv * uv_mask_size).clip(0, uv_mask_size - 1)
                pixel_coord = np.int32(pixel_coord)
                pixel_cls = uv_mask[pixel_coord[1], pixel_coord[0]]
                matched = pixel_cls == LIP_COLOR
                # loop_uv.select = matched
                full_loop = full_loop and matched
                # if matched:
                #    loop.vert.select_set(True)
            face.select_set(full_loop)

        # bm.select_mode = {'VERT', 'EDGE', 'FACE'}
        bm.select_flush_mode()
        # bpy.context.tool_settings.mesh_select_mode = (False, False, True)
        bpy.ops.uv.select_all(action='SELECT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.uv.select_split()

        bpy.ops.object.mode_set(mode='OBJECT')
        #  def separate_lips(self):
        base_layer_np = self.get_base_uv_layer_np()
        # pixel_class = get_pixel_class()
        selection = self.get_base_uv_layer_selection_np()
        base_layer_np[selection] = base_layer_np[selection] + LIP_TRANS
        self.update_base_uv_layer(base_layer_np)
        # += [0.043945, 0.006836] # top arm
        # += [-0.072266 , 0.085937] # obttom arm
        # += [0.008526, 0.019377] # torso
        # *= 0.25# nails
        # -= 0.5 # nails

    def subdivide_breast_bones(self):

        BODY_RIG = self.get_body_rig()
        bpy.ops.object.select_all(action='DESELECT')
        BODY_RIG.select_set(True)
        bpy.context.view_layer.objects.active = BODY_RIG
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.armature.select_all(action='DESELECT')
        cuts = 2
        for bone_name in ['r_pectoral', 'l_pectoral']:
            bone = BODY_RIG.data.edit_bones[bone_name]
            select_bone(bone)
        bpy.ops.armature.subdivide(number_cuts=cuts)
        l_pec_groups = []
        r_pec_groups = []
        pectorals = [('r_pectoral', r_pec_groups), ('l_pectoral', l_pec_groups)]
        for bone_name, vertex_groups in pectorals:
            for i in range(0, cuts):
                subbone_name = bone_name+"."+str(i+1).zfill(3)
                subbone = BODY_RIG.data.edit_bones[subbone_name]
                subbone_name = subbone.name = bone_name+str(i+1)
                group = bpy.context.object.vertex_groups.new(name=subbone_name)
                vertex_groups.append(group.index)
        BODY_M = self.get_body_mesh()
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.mode_set(mode='EDIT')

        def contains_group(vertex, group_index):
            for g in vertex.groups:
                if g.group == group_index:
                    return g.weight
            return 0

        l_pec_idx = BODY_M.vertex_groups['l_pectoral'].index
        r_pec_idx = BODY_M.vertex_groups['r_pectoral'].index
        l_pec_weights = np.array([contains_group(v, l_pec_idx) for v in BODY_M.data.vertices])
        r_pec_weights = np.array([contains_group(v, r_pec_idx) for v in BODY_M.data.vertices])
        for pec_weights, pec_groups in [(l_pec_weights, l_pec_groups), (r_pec_weights, r_pec_groups)]:
            max_weight = np.max(pec_weights)
            for i, subpec_group in enumerate(l_pec_groups):
                subpec_weights = pec_weights - (i+1)/(cuts+1)







    # *= 0.25# nails
    # -= 0.5 # nails


def save_textures(duf_filepath):
    duf_filepath = os.path.abspath(duf_filepath)
    workdir = os.path.dirname(duf_filepath)
    name = os.path.basename(duf_filepath)[:-len(".duf")]
    blend_filepath = os.path.join(workdir, name + ".blend")
    bpy.ops.wm.save_as_mainfile(filepath=blend_filepath)
    rig = find_body_rig()
    body = bpy.data.objects[rig.name + ' Mesh']
    bpy.ops.object.select_all(action='DESELECT')
    body.select_set(True)
    bpy.context.view_layer.objects.active = body
    bpy.ops.daz.save_local_textures()


def install_libraries():
    pil_missing = False
    opencv_missing = False
    try:
        import PIL
    except ModuleNotFoundError:
        pil_missing = True
    try:
        import cv2
    except ModuleNotFoundError:
        opencv_missing = True
    if pil_missing or opencv_missing:
        py_exe = sys.executable
        res_path = os.path.realpath(os.path.join(py_exe, "../../lib/site-packages"))
        print("Run the following command as an admin: ")
        print('&"' + py_exe + '" -m pip install pillow opencv-python "--target=' + res_path + '"')


def run_outside_blender():
    import subprocess

    file_path = os.path.realpath(__file__)

    if len(sys.argv) < 2 or not sys.argv[1].endswith(".duf"):
        print("Specify path to .duf")
        exit()
    duf_path: str = os.path.abspath(sys.argv[1])
    if not os.path.exists(duf_path):
        print("File not exists:", duf_path)
        exit()
    subprocess.run(["blender", "-P", file_path, "--", duf_path])

    # Image("")


bl_info = {
    "name": "Daz Optimizer",
    "blender": (2, 80, 0),
    "category": "Object",
}


class EasyImportPanel(bpy.types.Panel):
    bl_label = "Panel"
    bl_idname = "dazoptim_easy_import_panel"
    bl_space_type = 'FILE_BROWSER'
    bl_region_type = 'TOOLS'
    filepath = None
    directory = None

    @classmethod
    def poll(cls, context):
        op = context.active_operator
        if op and op.bl_idname == "DAZ_OT_easy_import_daz":
            cls.directory = op.directory
            cls.filepath = op.filepath
            context.scene['duf_filepath'] = op.filepath
        return False

    # needs a draw method
    def draw(self, context):
        pass


class DazDelCube_operator(bpy.types.Operator):
    """ Delete default cube """
    bl_idname = "dazoptim.delcube"
    bl_label = "Delete default cube"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        for x in list(bpy.data.objects):
            bpy.data.objects.remove(x)
        for x in list(bpy.data.collections):
            bpy.data.collections.remove(x)
        return {'FINISHED'}


class DazLoad_operator(bpy.types.Operator):
    """ Load daz character """
    bl_idname = "dazoptim.load"
    bl_label = "Load Daz character"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        bpy.ops.daz.easy_import_daz('INVOKE_DEFAULT',
                                    # filepath=self.duf_path,
                                    # files=[filepath],
                                    # directory="",
                                    filter_glob="*.duf;*.dsf;*.png;*.jpeg;*.jpg;*.bmp",
                                    fitMeshes='DBZFILE',
                                    materialMethod='EXTENDED_PRINCIPLED',
                                    useMergeMaterials=True,
                                    useEliminateEmpties=True,
                                    useMergeRigs=False,
                                    useApplyTransforms=False,
                                    useMergeToes=False,
                                    useFavoMorphs=False,
                                    useUnits=False,
                                    useExpressions=False,
                                    useVisemes=False,
                                    useHead=False,
                                    useFacs=False,
                                    useFacsdetails=False,
                                    useFacsexpr=False,
                                    useAnime=False,
                                    usePowerpose=False,
                                    useBody=False,
                                    useBulges=False,
                                    useJcms=True,
                                    ignoreFingers=True,
                                    useMasculine=False,
                                    useFeminine=False,
                                    useFlexions=True,
                                    useBakedCorrectives=True,
                                    useDazFavorites=True,
                                    useAdjusters=False,
                                    onMorphSuffix='SMART',
                                    useTransferFace=True,
                                    useTransferHair=False,
                                    useTransferGeografts=True,
                                    useTransferClothes=True,
                                    useTransferHD=True,
                                    useMergeGeografts=False,
                                    useMakePosable=True,
                                    useFinalOptimization=False,
                                    ignoreUrl=False,
                                    ignoreFinger=False,
                                    morphSuffix="",
                                    ignoreHdMorphs=False,
                                    useMhxOnly=False,
                                    duplicateDistance=1,
                                    useMergeNonConforming='CONTROLS',
                                    useConvertWidgets=True,
                                    useHiddenRigs=False,
                                    useMergeUvs=True,
                                    allowOverlap=False,
                                    keepOriginal=False,
                                    useFixTiles=True,
                                    useSubDDisplacement=True,
                                    useGeoNodes=False,
                                    morphStrength=1,
                                    skinColor=(0.6, 0.4, 0.25, 1),
                                    clothesColor=(0.09, 0.01, 0.015, 1),
                                    useApplyRestPoses=True,
                                    favoPath="")
        return {'FINISHED'}


class DazSave_operator(bpy.types.Operator):
    """ Load daz character """
    bl_idname = "dazoptim.save"
    bl_label = "Load Daz character"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        if bpy.types.dazoptim_easy_import_panel.filepath is None and 'duf_filepath' not in bpy.context.scene:
            self.report({"WARNING"}, "Load a DAZ character first!")
            return {'CANCELLED'}
        if bpy.types.dazoptim_easy_import_panel.filepath is not None and 'duf_filepath' not in bpy.context.scene:
            bpy.context.scene['duf_filepath'] = bpy.types.dazoptim_easy_import_panel.filepath
        save_textures(bpy.context.scene['duf_filepath'])
        return {'FINISHED'}


class DazConcatTextures_operator(bpy.types.Operator):
    """ Concatenate textures into one """
    bl_idname = "dazoptim.concat"
    bl_label = "Concatenate textures"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().concat_textures()

        return {'FINISHED'}


class DazOptimizeEyes_operator(bpy.types.Operator):
    """ Optimize eyes """
    bl_idname = "dazoptim.optim_eyes"
    bl_label = "Optimize eyes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().optimize_eyes()

        return {'FINISHED'}


class DazMergeGrografts_operator(bpy.types.Operator):
    """ Merge geografts """
    bl_idname = "dazoptim.merge_geografts"
    bl_label = "Merge geografts"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_geografts()

        return {'FINISHED'}


class DazMergeEyes_operator(bpy.types.Operator):
    """ Merge Eyes """
    bl_idname = "dazoptim.merge_eyes"
    bl_label = "Merge eyes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_eyes()

        return {'FINISHED'}


class DazMergeMouth_operator(bpy.types.Operator):
    """ Merge Mouth """
    bl_idname = "dazoptim.merge_mouth"
    bl_label = "Merge mouth"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_mouth()

        return {'FINISHED'}


class DazOptimizeUVs_operator(bpy.types.Operator):
    """ Optimize UVs """
    bl_idname = "dazoptim.optim_uvs"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazSeparateLipUVs_operator(bpy.types.Operator):
    """ Separate Lip UVs """
    bl_idname = "dazoptim.sep_lip_uvs"
    bl_label = "Separate Lip UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().separate_lip_uvs()

        return {'FINISHED'}


class DazAddBreastBones_operator(bpy.types.Operator):
    """ Add breast bones """
    bl_idname = "dazoptim.breast_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().subdivide_breast_bones()

        return {'FINISHED'}


class DazAddGluteBones_operator(bpy.types.Operator):
    """ Add glute bones """
    bl_idname = "dazoptim.glute_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazAddThighBones_operator(bpy.types.Operator):
    """ Add thigh bones """
    bl_idname = "dazoptim.thigh_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazFitClothes_operator(bpy.types.Operator):
    """ fit clothes """
    bl_idname = "dazoptim.fit_clothes"
    bl_label = "Fit clothes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazOptimizeHair_operator(bpy.types.Operator):
    """ Optimize hair """
    bl_idname = "dazoptim.optim_hair"
    bl_label = "Optimize Hair"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazConvertToUe5Skeleton_operator(bpy.types.Operator):
    """ Convert rig to UE5-compatible skeleton """
    bl_idname = "dazoptim.convert_ue5"
    bl_label = "Convert rig to UE5-compatible skeleton"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazSendToUnreal(bpy.types.Operator):
    """ Send to unreal """
    bl_idname = "dazoptim.send_to_ue5"
    bl_label = "Send to unreal"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazOptimize_sidebar(bpy.types.Panel):
    """DazOptim actions"""
    bl_label = "DazOptim"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "DazOptim"

    def draw(self, context):
        col = self.layout.column(align=True)
        i = 0
        for op_class, op_text in operators:
            prop = col.operator(op_class.bl_idname, text=str(i)+". "+op_text)
            i += 1


operators = [
    (DazDelCube_operator, "Delete default cube"),
    (DazLoad_operator, "Load Daz"),
    (DazSave_operator, "Save textures"),
    (DazOptimizeEyes_operator,  "Optimize eyes"),
    (DazMergeGrografts_operator, "Merge Geografts"),
    (DazMergeEyes_operator,  "Merge eyes"),
    (DazMergeMouth_operator, "Merge mouth"),
    (DazConcatTextures_operator, "Merge textures"),
    (DazOptimizeUVs_operator, "Optimize UVs"),
    (DazSeparateLipUVs_operator, "Separate Lip UVs"),
    (DazAddBreastBones_operator, "Subdivide breast bones"),
    (DazAddGluteBones_operator, "Add glute bones"),
    (DazAddThighBones_operator, "Add thigh bones"),
    (DazFitClothes_operator, "Fit Clothes"),
    (DazOptimizeHair_operator, "Optimize hair"),
    (DazConvertToUe5Skeleton_operator, "Convert to UE5 Skeleton"),
    (DazSendToUnreal, "Send to unreal"),
]

classes = [
    DazOptimize_sidebar,
    EasyImportPanel,
] + [op for (op, _) in operators]


def register():
    for c in classes:
        bpy.utils.register_class(c)


def unregister():
    for c in classes:
        bpy.utils.unregister_class(c)


if __name__ == '__main__':
    register()
